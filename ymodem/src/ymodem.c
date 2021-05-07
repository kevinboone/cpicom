/*=========================================================================

  Ymodem

  ymodem/ymodem.c

  (c)2021 Kevin Boone, GPLv3.0

  Based on work placed into the public domain by 
  Fredrik Hederstierna, 2014. I'll tidy this up and document it properly,
  if I ever get time. Right now, it works, and I don't want to fiddle
  with it.

=========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include "pico/stdlib.h" 
#include "files/compat.h" 
#include "log/log.h"
#include <files/filecontext.h>
#include <files/fileutil.h>
#include <ymodem/ymodem.h>

#define YM_FILE_SIZE_LENGTH        16
#define FYMODEM_FILE_NAME_MAX_LENGTH  64

/* Packet constants */
#define YM_PACKET_SEQNO_INDEX      1
#define YM_PACKET_SEQNO_COMP_INDEX 2
#define YM_PACKET_HEADER           3      
#define YM_PACKET_TRAILER          2   
#define YM_PACKET_OVERHEAD         (YM_PACKET_HEADER + YM_PACKET_TRAILER)
#define YM_PACKET_SIZE             128
#define YM_PACKET_1K_SIZE          1024
#define YM_PACKET_RX_TIMEOUT_MS    1000
#define YM_PACKET_ERROR_MAX_NBR    5

/* Contants defined by YModem protocol */
#define YM_SOH                     0x01
#define YM_STX                     0x02 
#define YM_EOT                     0x04 
#define YM_ACK                     0x06 
#define YM_NAK                     0x15 
#define YM_CAN                     0x18 
#define YM_CRC                     0x43 
#define YM_ABT1                    0x41 
#define YM_ABT2                    0x61 

/*=========================================================================

  write_log 

=========================================================================*/
static void write_log (const char *fmt,...)
  {
  /*
  va_list ap;
  va_start (ap, fmt);
  MYFILE *f = my_fopen ("ymlog.txt", "a");
  char s[256];
  vsprintf (s, fmt, ap);
  my_fwrite (s, 1, strlen (s), f);
  my_fwrite ("\n", 1, 1, f);
  my_fclose (f);
  va_end (ap);
  */
  }

/*=========================================================================

  my_sleep_ms 

=========================================================================*/
static void my_sleep_ms (uint32_t val)
  {
#if PICO_ON_DEVICE
  sleep_ms (val);
#else
  usleep (val * 1000);
#endif
  }

/*=========================================================================

  ymodem_crc16

=========================================================================*/
static uint16_t ymodem_crc16 (const uint8_t *buf, uint16_t len)
  {
  uint16_t x;
  uint16_t crc = 0;
  while (len--) 
    {
    x = (crc >> 8) ^ *buf++;
    x ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }
  return crc;
  }

/*=========================================================================

  ymodem_read_32

=========================================================================*/
static void ymodem_read_32 (const uint8_t* buf, uint32_t *val)
  {
  const uint8_t *s = buf;
  uint32_t res = 0;
  uint8_t c;
  /* Trim and strip leading spaces if any */
  do 
    {
    c = *s++;
    } while (c == ' ');
  while ((c >= '0') && (c <= '9')) 
    {
    c -= '0';
    res *= 10;
    res += c;
    c = *s++;
    }
  *val = res;
  }

/*=========================================================================

  ymodem_rx_packet

  Receive a single packet. Note that, in principle, successive packets
  can be of different size. This function sets the packet size into
  *rxlen in a successful receive. On a failure, *rxlen will be <= 0.

  Returns 0 on success, -1 on a timeout, 1 on a cancel or broken packet.
      -1: timeout or packet error
       1: abort by user / corrupt packet

=========================================================================*/
static int32_t ymodem_rx_packet (ConsoleParams *console_params, 
    uint8_t *rxdata, int32_t *rxlen, uint32_t packets_rxed,
    uint32_t timeout_ms)
  {
  *rxlen = 0;

  write_log ("Start ym_rx_packet");
  int32_t c = console_params->console_get_char_timeout 
     (console_params->context, timeout_ms);
  write_log ("Got a character");
  if (c < 0) 
    {
    /* end of stream */
    write_log ("ym_rx_packet end of stream");
    return -1;
    }

  uint32_t rx_packet_size;

  switch (c) 
    {
    case YM_SOH:
      rx_packet_size = YM_PACKET_SIZE;
      write_log ("Got SOH");
      break;
    case YM_STX:
      write_log ("Got STX");
      rx_packet_size = YM_PACKET_1K_SIZE;
      break;
    case YM_EOT:
      /* ok */
      write_log ("EOT");
      return 0;
    case YM_CAN:
      write_log ("YM_CAN");
      c = console_params->console_get_char_timeout 
            (console_params->context, timeout_ms);
      if (c == YM_CAN) 
        {
        write_log ("YM_CAN 2");
        *rxlen = -1;
        /* ok */
        return 0;
        }
      /* fall-through */
    case YM_CRC:
      write_log ("YM_CRC");
      if (packets_rxed == 0) 
        {
        /* could be start condition, first byte */
        return 1;
        }
     /* fall-through */
    case YM_ABT1:
    case YM_ABT2:
      /* User try abort, 'A' or 'a' received */
      write_log ("YM_ABT");
      return 1;
    default:
      /* This case could be the result of corruption on the first octet
         of the packet, but it's more likely that it's the user banging
         on the terminal trying to abort a transfer. Technically, the
         former case deserves a NAK, but for now we'll just treat this
         as an abort case. */
      write_log ("YM default");
      *rxlen = -1;
      return 0;
    }

  /* store data RXed */
  *rxdata = (uint8_t)c;

  uint32_t i;
  for (i = 1; i < (rx_packet_size + YM_PACKET_OVERHEAD); i++) 
    {
    c = console_params->console_get_char_timeout 
       (console_params->context, timeout_ms);
    if (c < 0) 
      {
      /* end of stream */
      return -1;
      }
    /* store data RXed */
    rxdata[i] = (uint8_t)c;
    }

  /* just a sanity check on the sequence number/complement value.
     caller should check for in-order arrival. */
  uint8_t seq_nbr = (rxdata[YM_PACKET_SEQNO_INDEX] & 0xff);
  uint8_t seq_cmp = ((rxdata[YM_PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff);
  if (seq_nbr != seq_cmp) 
    {
    /* seq nbr error */
    write_log ("Seq nbr error");
    return 1;
    }

  /* check CRC16 match */
  uint16_t crc16_val = ymodem_crc16 
     ((const unsigned char *)(rxdata + YM_PACKET_HEADER),
                                rx_packet_size + YM_PACKET_TRAILER);
  if (crc16_val) 
    {
    write_log ("CRC Val != 0");
    return 1;
    }

  *rxlen = rx_packet_size;
  /* success */
  write_log ("Return OK");
  return 0;
  }

/*=========================================================================

  ymodem_receive_

=========================================================================*/
YmodemErr ymodem_receive_ (ConsoleParams *console_params, 
     const char *out_filename, uint32_t maxsize)
  {
  YmodemErr err = 0;

  // Ensure we can write the output file, if specified. Although it
  //   will get created later, it's better to find out now, rather
  //   than in the middle of a long upload, that we can't write the
  //   file.
  if (out_filename)
    {
    if (fileutil_write_file (out_filename, (uint8_t*)"", 0) != 0)
      {
      log_debug ("ym_recv: fail wrt: %s", out_filename);
      return YmodemWriteFile;
      }
    }

  /* Alloc 1k on stack -- might need to think about this */
  uint8_t rx_packet_data [YM_PACKET_1K_SIZE + YM_PACKET_OVERHEAD];
  int32_t rx_packet_len;

  uint8_t filesize_asc[YM_FILE_SIZE_LENGTH];
  uint32_t filesize = 0;

  BOOL first_try = TRUE;
  BOOL session_done = FALSE;

  // Filename read from header
  char filename [MAX_FNAME]; 
  strcpy (filename, "untitled.txt");

  uint32_t nbr_errors = 0;

  filename[0] = 0;
  
  // Receive files
  do 
    {
    write_log ("Start of loop");
    if (first_try) 
      console_params->console_out_char (console_params->context, YM_CRC);

    first_try = FALSE;

    BOOL crc_nak = TRUE;
    BOOL file_done = FALSE;
    uint32_t packets_rxed = 0;

    uint32_t total_written = 0;
    do 
      { // ! file done 
      // Receive packets 
      int32_t res = ymodem_rx_packet (console_params, rx_packet_data,
                        &rx_packet_len, packets_rxed, 
			YM_PACKET_RX_TIMEOUT_MS);
      switch (res) 
        {
        case 0: 
          {
          // Packet received, clear packet error counter 
          nbr_errors = 0;
          switch (rx_packet_len) 
            {
            case -1: 
              {
              /* aborted by sender */
              console_params->console_out_char 
	        (console_params->context, YM_ACK);
              write_log ("Cancelled -- send ACK");
              return YmodemCancelled;
              }
            case 0: 
              {
              /* EOT - End Of Transmission */
              console_params->console_out_char 
	         (console_params->context, YM_ACK);
              write_log ("EOT -- send ACK");
              /* TODO: Add some sort of sanity check on the number of
                 packets received and the advertised file length. */
              file_done = TRUE;
              /* TODO: set first_try here to resend C ? */
              break;
              }
            default: 
              {
              /* normal packet, check sequence number */
              write_log ("Normal pkt");
              uint8_t seq_nbr = rx_packet_data [YM_PACKET_SEQNO_INDEX];
              if (seq_nbr != (packets_rxed & 0xff)) 
                {
                /* wrong sequence number */
                console_params->console_out_char 
		  (console_params->context, YM_NAK);
                write_log ("Bad seq -- send NAK");
                } 
              else 
                {
                write_log ("Seq num OK");
                if (packets_rxed == 0) 
                  {
                  write_log ("No pkts so far");
                  /* The spec suggests that the whole data section should
                  be zeroed, but some senders might not do this.
                  If we have a NULL filename and the first few digits of
                  the file length are zero, then call it empty. */
                  int32_t i;
                  for (i = YM_PACKET_HEADER; i < YM_PACKET_HEADER + 4; i++) 
                    {
                    if (rx_packet_data[i] != 0) 
                      {
                      break;
                      }
                    }
                  /* non-zero bytes found in header: 
		     filename packet has data */
                  if (i < YM_PACKET_HEADER + 4) 
                    {
                    // Read file name 
                    uint8_t *file_ptr = 
		      (uint8_t*)(rx_packet_data + YM_PACKET_HEADER);
                    i = 0;
                    while (*file_ptr && (i < MAX_FNAME)) 
                      {
                      filename[i++] = *file_ptr++;
                      }
                    filename[i++] = '\0';

                    if (fileutil_write_file (filename, (uint8_t*)"", 0) 
		           != 0)
                      {
		      // Ensure file exists. But if it can't be created,
		      //   what on earth can we do here?
		      }
                    file_ptr++;
                    // Read file size 
                    i = 0;
                    while ((*file_ptr != ' ') && (i < YM_FILE_SIZE_LENGTH)) 
                      {
                      filesize_asc[i++] = *file_ptr++;
                      }
                    filesize_asc[i++] = '\0';
                    // Convert file size
                    ymodem_read_32 (filesize_asc, &filesize);
                    // Check file size 
                    if (filesize > maxsize) 
                      {
		      err = YmodemTooBig;
                      goto rx_err_handler;
                      }
                    write_log ("???");
                    console_params->console_out_char 
		      (console_params->context, YM_ACK);
                    console_params->console_out_char 
		      (console_params->context, crc_nak ? YM_CRC : YM_NAK);
                    crc_nak = FALSE;
                    }
                  else 
                    {
                    // Filename packet is empty, end session 
                    // TODO -- carry on if no filename
                    write_log ("Filename pkt empty, but send ACK");
                    console_params->console_out_char 
		      (console_params->context, YM_ACK);
                    file_done = TRUE;
                    session_done = TRUE;
                    break;
                    }
                  }
                else 
                  {
                  write_log ("Not first pkt");
                  int to_write = rx_packet_len;
                  if (total_written + rx_packet_len > filesize)
                    to_write = filesize - total_written;
		  const char *current_filename;
		  if (out_filename)
		    current_filename = out_filename;
		  else
		    current_filename = filename;
                  write_log ("About to append");
                  err = fileutil_append_file (current_filename, 
                    rx_packet_data + YM_PACKET_HEADER, to_write); 
		  if (err) goto rx_err_handler;
                  write_log ("Appended file -- write ACK");
                  console_params->console_out_char 
		    (console_params->context, YM_ACK);
                  total_written += rx_packet_len;
                  }
                packets_rxed++;
                }  /* sequence number check ok */
              } /* default */
            } /* inner switch */
          break;
          } /* case 0 */
        default: 
	  {
          /* ym_rx_packet() returned error */
          if (packets_rxed > 0) 
	    {
            nbr_errors++;
            if (nbr_errors >= YM_PACKET_ERROR_MAX_NBR) 
	      {
	      err = YmodemBadPacket;
              goto rx_err_handler;
              }
            }
          console_params->console_out_char 
	    (console_params->context, YM_CRC);
          break;
          } //  default 
        } // switch
      } while (! file_done);
    } while (! session_done);

  return 0;

  // Ugh. Ugly jump target. But the logic is too twisted to recover
  //   any other way.
  rx_err_handler:
 
  console_params->console_out_char (console_params->context, YM_CAN);
  console_params->console_out_char (console_params->context, YM_CAN);
  my_sleep_ms (1000);
  
  return err;
  }

/*=========================================================================

  ymodem_receive

=========================================================================*/
YmodemErr ymodem_receive (ConsoleParams *console_params, 
     const char *out_filename, uint32_t maxsize)
  {
#if PICO_ON_DEVICE
  stdio_set_translate_crlf (&stdio_usb, false);
#endif

  YmodemErr ret = ymodem_receive_ (console_params, out_filename, maxsize);

#if PICO_ON_DEVICE
  stdio_set_translate_crlf (&stdio_usb, true);
#endif
 
  return ret;
  }

/*=========================================================================

  ymodem_writeU32

  // TODO: can we just use sprintf?

=========================================================================*/
static uint32_t ymodem_writeU32 (uint32_t val, uint8_t *buf)
  {
  uint32_t ci = 0;
  if (val == 0) 
    {
    /* If already zero then just return zero */
    buf[ci++] = '0';
    }
  else 
    {
    uint8_t s[11];
    int32_t i = sizeof(s) - 1;
    /* z-terminate string */
    s[i] = 0;
    while ((val > 0) && (i > 0)) 
      {
      /* write decimal char */
      s[--i] = (val % 10) + '0';
      val /= 10;
      }
    
    uint8_t *sp = &s[i];
    
    /* copy results to out buffer */
    while (*sp) 
      {
      buf[ci++] = *sp++;
      }
    }
  /* z-term */
  buf[ci] = 0;
  /* return chars written */
  return ci;
  }

/*=========================================================================

  ymodem_send_packet

=========================================================================*/
static void ymodem_send_packet (ConsoleParams *cp, 
              uint8_t *txdata, int32_t block_nbr)
  {
  int32_t tx_packet_size;

  /* We use a short packet for block 0, all others are 1K */
  if (block_nbr == 0) 
    {
    tx_packet_size = YM_PACKET_SIZE;
    }
  else 
    {
    tx_packet_size = YM_PACKET_1K_SIZE;
    }

  uint16_t crc16_val = ymodem_crc16 (txdata, tx_packet_size);

  /* For 128 byte packets use SOH, for 1K use STX */
  cp->console_out_char (cp->context,  (block_nbr == 0) ? YM_SOH : YM_STX );
  /* write seq numbers */
  cp->console_out_char (cp->context, block_nbr & 0xFF);
  cp->console_out_char (cp->context, ~block_nbr & 0xFF);

  /* write txdata */
  int32_t i;
  for (i = 0; i < tx_packet_size; i++) 
    {
    cp->console_out_char (cp->context, txdata[i]);
    }

  /* write crc16 */
  write_log ("my crc is %04X", crc16_val);
  cp->console_out_char (cp->context, (crc16_val >> 8) & 0xFF);
  cp->console_out_char (cp->context, crc16_val & 0xFF);
  }


/*=========================================================================

  ymodem_send_packet0

=========================================================================*/
static void ymodem_send_packet0 (ConsoleParams *cp, const char* filename,
                            int32_t filesize)
  {
  int32_t pos = 0;
  /* put 256byte on stack, ok? reuse other stack mem? */
  uint8_t block [YM_PACKET_SIZE];
  if (filename) 
    {
    /* write filename */
    while (*filename && (pos < YM_PACKET_SIZE - YM_FILE_SIZE_LENGTH - 2)) 
      {
      block[pos++] = *filename++;
      }
    /* z-term filename */
    block[pos++] = 0;
    
    /* write size, TODO: check if buffer can overwritten here. */
    pos += ymodem_writeU32 (filesize, &block[pos]);
    }

  /* z-terminate string, pad with zeros */
  while (pos < YM_PACKET_SIZE) 
    {
    block[pos++] = 0;
    }
  
  /* send header block */
  ymodem_send_packet (cp, block, 0);
  }


/*=========================================================================

  ymodem_send_data_packets

=========================================================================*/
static void ymodem_send_data_packets (ConsoleParams *cp, uint8_t* txdata,
                                 uint32_t txlen,
                                 uint32_t timeout_ms)
  {
  write_log ("Entering ym_snd_data_pkts");
  int32_t block_nbr = 1;
  
  while (txlen > 0) 
    {
    write_log ("txlen is %d", (int)txlen);
    /* check if send full 1k packet */
    uint32_t send_size;
    if (txlen > YM_PACKET_1K_SIZE) 
      {
      send_size = YM_PACKET_1K_SIZE;
      } 
    else 
      {
      send_size = txlen;
      }
    write_log ("send size is %d", (int)send_size);
    /* send packet */
    ymodem_send_packet (cp, txdata, block_nbr);
    int32_t c = cp->console_get_char_timeout (cp->context, timeout_ms);
    write_log ("Sent packet, got %d '%c'", c, c); 
    switch (c) 
      {
      case YM_ACK: 
        {
        write_log ("Got ACK");
        txdata += send_size;
        txlen -= send_size;
        block_nbr++;
        break;
        }
      case -1:
      case YM_CAN: 
        {
        write_log ("Got CAN");
        return;
        }
      default:
        write_log ("Got neither ACK nor CAN");
        break;
      }
    }
  
  write_log ("Done file loop");
  int32_t ch;
  do 
    {
    cp->console_out_char (cp->context, YM_EOT);
    ch = cp->console_get_char_timeout (cp->context, timeout_ms);
    } while ((ch != YM_ACK) && (ch != -1));
  
  /* send last data packet */
  if (ch == YM_ACK) 
    {
    ch = cp->console_get_char_timeout (cp->context, timeout_ms);
    if (ch == YM_CRC) 
      {
      do 
        {
        ymodem_send_packet0 (cp, 0, 0);
        ch = cp->console_get_char_timeout (cp->context, timeout_ms);
        } while ((ch != YM_ACK) && (ch != -1));
      }
    }
  write_log ("Leaving ym_snd_data_pkts");
  }



/*=========================================================================

  ymodem_send_data

=========================================================================*/
YmodemErr ymodem_send_data (ConsoleParams *cp, 
          uint8_t *txdata, uint32_t txsize, const char *filename)
  {
  write_log ("Entering ym_send_data");
  YmodemErr err = 0;
  /* not in the specs, send CRC here just for balance */
  int32_t ch = YM_CRC;
  do 
    {
    cp->console_out_char (cp->context, YM_CRC);
    ch = cp->console_get_char_timeout (cp->context, 1000);
    } while (ch < 0);

  /* we do require transfer with CRC */
  if (ch != YM_CRC) 
    {
    err = YmodemNoCRC;
    goto tx_err_handler;
    } 

  bool crc_nak = true;
  bool file_done = false;
  do 
    {
    write_log ("About to send pkt 0");
    ymodem_send_packet0 (cp, filename, txsize);
    /* When the receiving program receives this block and successfully
       opened the output file, it shall acknowledge this block with an ACK
       character and then proceed with a normal XMODEM file transfer
       beginning with a "C" or NAK tranmsitted by the receiver. */
    ch = cp->console_get_char_timeout (cp->context, YM_PACKET_RX_TIMEOUT_MS);
    write_log ("Sent pkt 0, got %d '%c'", ch, ch);

    if (ch == YM_ACK) 
      {
      write_log ("Got ACK");
      ch = cp->console_get_char_timeout (cp->context, YM_PACKET_RX_TIMEOUT_MS);

      if (ch == YM_CRC) 
        {
        write_log ("Got CRC after ACK");
        ymodem_send_data_packets (cp, txdata, txsize, YM_PACKET_RX_TIMEOUT_MS);
        /* success */
        file_done = true;
        }
      else if (ch == YM_CAN)
        {
        write_log ("Got CAN after ACK");
	err = YmodemCancelled;
	goto tx_err_handler;
	}
      }
    else if ((ch == YM_CRC) && (crc_nak)) 
      {
      write_log ("Got CRC after pkt 0");
      crc_nak = false;
      continue;
      }
    else if ((ch != YM_NAK) || (crc_nak)) 
      {
      write_log ("Got NAK after pkt 0");
      err = YmodemBadPacket;
      goto tx_err_handler;
      }
    } while (!file_done);

  write_log ("Leaving ym_send_data -- OK");
  return 0;

  tx_err_handler:

  cp->console_out_char (cp->context, YM_CAN);
  cp->console_out_char (cp->context, YM_CAN);
  my_sleep_ms (1000);
  write_log ("Leaving ym_send_data -- Error");
  return err;
  }



/*=========================================================================

  ymodem_send

=========================================================================*/
YmodemErr ymodem_send (ConsoleParams *cp, const char *filename)
  {
  YmodemErr err = 0;
  int fd = my_open (filename, O_RDONLY);
  if (fd >= 0)
    {
    // I can't tell you how many hours I wasted, tracing serial packets
    // and what-not, before I realized that the stdio code in the Pico
    // SDK was fiddling with cr/lf translation.
#if PICO_ON_DEVICE
    stdio_set_translate_crlf (&stdio_usb, false);
#endif

    struct stat sb;
    filecontext_global_fstat (fd, &sb);
    uint32_t txsize = sb.st_size; 
    // TODO -- split basename off filename

    // This is a bit ugly -- pad read the whole file into memory,
    //   into a zero'd area that will accomodate the whole file, 
    //   and be an integer number of 1k blocks. This is nasty, but
    //   it's easier than trying to work out how to calculate the
    //   CRC differently for each block; and its easier to move
    //   a memory pointer around if retries are needed, that to fiddle
    //   with file offsets. Still -- ugh :/
    
    int pkts = txsize / YM_PACKET_1K_SIZE;
    pkts++;

    uint32_t rounded_size = pkts * YM_PACKET_1K_SIZE;

    write_log ("rnd size=%d", rounded_size);

    uint8_t *buff = malloc (rounded_size);

    memset (buff, 0, rounded_size);

    int n = my_read (fd, buff, txsize);
    write_log ("Read %d bytes from file", n);

    err = ymodem_send_data (cp, buff, txsize, filename);

    free (buff);

    my_close (fd);

#if PICO_ON_DEVICE
    stdio_set_translate_crlf (&stdio_usb, true);
#endif
    }
  else
    err = YmodemReadFile;
  return err;
  }

/*=========================================================================

  ymodem_strerror

=========================================================================*/
const char *ymodem_strerror (YmodemErr err)
  {
  switch (err)
    {
    case YmodemOK: return "OK";
    case YmodemWriteFile: return "Can't write file";
    case YmodemReadFile: return "Can't read file";
    case YmodemTooBig: return "File too large";
    case YmodemChecksum: return "Bad checksum";
    case YmodemCancelled: return "Transfer cancelled";
    case YmodemBadPacket: return "Corrupt packet";
    case YmodemNoCRC: return "Sender does not support CRC";
    }
  return "OK";
  }


