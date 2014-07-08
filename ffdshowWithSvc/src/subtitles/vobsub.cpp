/*
 * Some code freely inspired from VobSub <URL:http://vobsub.edensrising.com>,
 * with kind permission from Gabest <gabest@freemail.hu>
 *
 * to ffdshow imported from mplayer
 */
#include "stdafx.h"
#include "ffdebug.h"
#include "Tstream.h"
#include "vobsub.h"
#include "spudec.h"
#include "IffdshowBase.h"
#include "Tsubreader.h"

/**********************************************************************/
Tstream* Tvobsub::rar_open(const char_t *flnm)
{
 if (fileexists(flnm))
  return new TstreamFile(flnm,true,false);
 else
  {
   char_t dsk[MAX_PATH],dir[MAX_PATH],name[MAX_PATH],ext[MAX_PATH];
   _splitpath_s(flnm, dsk, countof(dsk), dir, countof(dir), name, countof(name), ext, countof(ext));
   char_t flnmrar[MAX_PATH];
   _makepath_s(flnmrar, countof(flnmrar), dsk, dir, name, _l("rar"));
   char_t flnmrarsub[MAX_PATH];
   _makepath_s(flnmrarsub, countof(flnmrarsub), NULL, NULL, name, ext);
   return new TstreamRAR(flnmrar,flnmrarsub,config);
  }
}

/**********************************************************************
 * MPEG parsing
 **********************************************************************/

Tvobsub::mpeg_t* Tvobsub::mpeg_open(const char_t *filename)
{
    mpeg_t *res = (mpeg_t*)malloc(sizeof(mpeg_t));
    int err = res == NULL;
    if (!err) {
        res->pts = 0;
        res->aid = -1;
        res->packet = NULL;
        res->packet_size = 0;
        res->packet_reserve = 0;
        res->stream = rar_open(filename);
        err = res->stream == NULL;
        if (err)
            perror("fopen Vobsub file failed");
        if (err)
            free(res);
    }
    return err ? NULL : res;
}

void Tvobsub::mpeg_free(mpeg_t *mpeg)
{
    if (mpeg->packet)
        free(mpeg->packet);
    if (mpeg->stream)
        delete mpeg->stream;
    free(mpeg);
}

int Tvobsub::mpeg_eof(mpeg_t *mpeg)
{
    return mpeg->stream->eof();
}

long Tvobsub::mpeg_tell(mpeg_t *mpeg)
{
    return mpeg->stream->tell();
}

int Tvobsub::mpeg_run(mpeg_t *mpeg)
{
    unsigned int len, idx, version;
    int c;
    /* Goto start of a packet, it starts with 0x000001?? */
    const unsigned char wanted[] = { 0, 0, 1 };
    unsigned char buf[5];

    mpeg->aid = -1;
    mpeg->packet_size = 0;
    if (mpeg->stream->read(buf, 4, 1) != 1)
        return -1;
    while (memcmp(buf, wanted, sizeof(wanted)) != 0) {
        c = mpeg->stream->getc();
        if (c < 0)
            return -1;
        memmove(buf, buf + 1, 3);
        buf[3] = (unsigned char)c;
    }
    switch (buf[3]) {
    case 0xb9:                  /* System End Code */
        break;
    case 0xba:                  /* Packet start code */
        c = mpeg->stream->getc();
        if (c < 0)
            return -1;
        if ((c & 0xc0) == 0x40)
            version = 4;
        else if ((c & 0xf0) == 0x20)
            version = 2;
        else {
            DPRINTF(_l("VobSub: Unsupported MPEG version: 0x%02x"), c);
            return -1;
        }
        if (version == 4) {
            if (mpeg->stream->seek(9, SEEK_CUR))
                return -1;
        }
        else if (version == 2) {
            if (mpeg->stream->seek(7, SEEK_CUR))
                return -1;
        }
        else
            return -1;
        break;
    case 0xbd:                  /* packet */
        if (mpeg->stream->read(buf, 2, 1) != 1)
            return -1;
        len = buf[0] << 8 | buf[1];
        idx = mpeg_tell(mpeg);
        c = mpeg->stream->getc();
        if (c < 0)
            return -1;
        if ((c & 0xC0) == 0x40) { /* skip STD scale & size */
            if (mpeg->stream->getc() < 0)
                return -1;
            c = mpeg->stream->getc();
            if (c < 0)
                return -1;
        }
        if ((c & 0xf0) == 0x20) { /* System-1 stream timestamp */
            /* Do we need this? */
            return -1;
        }
        else if ((c & 0xf0) == 0x30) {
            /* Do we need thid? */
            return -1;
        }
        else if ((c & 0xc0) == 0x80) { /* System-2 (.VOB) stream */
            unsigned int pts_flags, hdrlen, dataidx;
            c = mpeg->stream->getc();
            if (c < 0)
                return -1;
            pts_flags = c;
            c = mpeg->stream->getc();
            if (c < 0)
                return -1;
            hdrlen = c;
            dataidx = mpeg_tell(mpeg) + hdrlen;
            if (dataidx > idx + len) {
                DPRINTF( _l("Invalid header length: %d (total length: %d, idx: %d, dataidx: %d)"), hdrlen, len, idx, dataidx);
                return -1;
            }
            if ((pts_flags & 0xc0) == 0x80) {
                if (mpeg->stream->read(buf, 5, 1) != 1)
                    return -1;
                if (!(((buf[0] & 0xf0) == 0x20) && (buf[0] & 1) && (buf[2] & 1) &&  (buf[4] & 1))) {
                    DPRINTF( _l("vobsub PTS error: 0x%02x %02x%02x %02x%02x "),buf[0], buf[1], buf[2], buf[3], buf[4]);
                    mpeg->pts = 0;
                }
                else
                    mpeg->pts = ((buf[0] & 0x0e) << 29 | buf[1] << 22 | (buf[2] & 0xfe) << 14
                        | buf[3] << 7 | (buf[4] >> 1));
            }
            else /* if ((pts_flags & 0xc0) == 0xc0) */ {
                /* what's this? */
                /* abort(); */
            }
            mpeg->stream->seek(dataidx, SEEK_SET);
            mpeg->aid = mpeg->stream->getc();
            if (mpeg->aid < 0) {
                DPRINTF( _l("Bogus aid %d"), mpeg->aid);
                return -1;
            }
            mpeg->packet_size = len - ((unsigned int) mpeg_tell(mpeg) - idx);
            if (mpeg->packet_reserve < mpeg->packet_size) {
                if (mpeg->packet)
                    free(mpeg->packet);
                mpeg->packet = (unsigned char*)malloc(mpeg->packet_size);
                if (mpeg->packet)
                    mpeg->packet_reserve = mpeg->packet_size;
            }
            if (mpeg->packet == NULL) {
                DPRINTF(_l("malloc failure"));
                mpeg->packet_reserve = 0;
                mpeg->packet_size = 0;
                return -1;
            }
            if (mpeg->stream->read(mpeg->packet, mpeg->packet_size, 1) != 1) {
                DPRINTF(_l("fread failure"));
                mpeg->packet_size = 0;
                return -1;
            }
            idx = len;
        }
        break;
    case 0xbe:                  /* Padding */
        if (mpeg->stream->read(buf, 2, 1) != 1)
            return -1;
        len = buf[0] << 8 | buf[1];
        if (len > 0 && mpeg->stream->seek(len, SEEK_CUR))
            return -1;
        break;
    default:
        if (0xc0 <= buf[3] && buf[3] < 0xf0) {
            /* MPEG audio or video */
            if (mpeg->stream->read(buf, 2, 1) != 1)
                return -1;
            len = buf[0] << 8 | buf[1];
            if (len > 0 && mpeg->stream->seek(len, SEEK_CUR))
                return -1;

        }
        else {
            DPRINTF(_l("unknown header 0x%02X%02X%02X%02X"),buf[0], buf[1], buf[2], buf[3]);
            return -1;
        }
    }
    return 0;
}

/**********************************************************************
 * Packet queue
 **********************************************************************/

void Tvobsub::packet_t::packet_construct(void)
{
    pts100 = 0;
    filepos = 0;
    size = 0;
    data = NULL;
}

void Tvobsub::packet_t::packet_destroy(void)
{
    if (data)
      free(data);
}

void Tvobsub::packet_queue_t::packet_queue_construct(void)
{
    id = NULL;
    altid = NULL;
    packets = NULL;
    packets_reserve = 0;
    packets_size = 0;
    current_index = 0;
}

void Tvobsub::packet_queue_t::packet_queue_destroy(void)
{
    if (packets) {
        while (packets_size--)
            (packets + packets_size)->packet_destroy();
        free(packets);
    }
    if (id) free(id);id=NULL;
    if (altid) free(altid);altid=NULL;
    return;
}

/* Make sure there is enough room for needed_size packets in the
   packet queue. */
int Tvobsub::packet_queue_t::packet_queue_ensure(unsigned int needed_size)
{
    if (packets_reserve < needed_size) {
        if (packets) {
            packet_t *tmp = (packet_t*)realloc(packets, 2 * packets_reserve * sizeof(packet_t));
            packets = tmp;
            packets_reserve *= 2;
        }
        else {
            packets = (packet_t*)malloc(sizeof(packet_t));
            packets_reserve = 1;
        }
    }
    return 0;
}

/* add one more packet */
int Tvobsub::packet_queue_t::packet_queue_grow(void)
{
    if (packet_queue_ensure(packets_size + 1) < 0)
        return -1;
    (packets + packets_size)->packet_construct();
    ++packets_size;
    return 0;
}

/* insert a new packet, duplicating pts from the current one */
int Tvobsub::packet_queue_t::packet_queue_insert(void)
{
    packet_t *pkts;
    if (packet_queue_ensure(packets_size + 1) < 0)
        return -1;
    /* XXX packet_size does not reflect the real thing here, it will be updated a bit later */
    memmove(packets + current_index + 2,
            packets + current_index + 1,
            sizeof(packet_t) * (packets_size - current_index - 1));
    pkts = packets + current_index;
    ++packets_size;
    ++current_index;
    (pkts + 1)->packet_construct();
    pkts[1].pts100 = pkts[0].pts100;
    pkts[1].filepos = pkts[0].filepos;
    return 0;
}

/**********************************************************************
 * Vobsub
 **********************************************************************/

/* Make sure that the spu stream idx exists. */
int Tvobsub::vobsub_ensure_spu_stream(unsigned int index)
{
    if (index >= spu_streams_size) {
        /* This is a new stream */
        if (spu_streams) {
            packet_queue_t *tmp = (packet_queue_t*)realloc(spu_streams, (index + 1) * sizeof(packet_queue_t));
            if (tmp == NULL) {
                DPRINTF(_l("vobsub_ensure_spu_stream: realloc failure"));
                return -1;
            }
            spu_streams = tmp;
        }
        else {
            spu_streams = (packet_queue_t*)malloc((index + 1) * sizeof(packet_queue_t));
            if (spu_streams == NULL) {
                DPRINTF(_l("vobsub_ensure_spu_stream: malloc failure"));
                return -1;
            }
        }
        while (spu_streams_size <= index) {
            (spu_streams + spu_streams_size)->packet_queue_construct();
            ++spu_streams_size;
        }
    }
    return 0;
}

Tvobsub::PARSE_RES Tvobsub::vobsub_add_id( const char *id, size_t idlen, const unsigned int index)
{
    if (vobsub_ensure_spu_stream( index) < 0)
        return PARSE_ERROR;
    if (id && idlen) {
        if (spu_streams[index].id)
            free(spu_streams[index].id);
        spu_streams[index].id = (char_t*)calloc(idlen + 1,sizeof(char_t));
        if (spu_streams[index].id == NULL) {
            DPRINTF(_l("vobsub_add_id: malloc failure"));
            return PARSE_ERROR;
        }
        //memcpy(spu_streams[index].id, id, idlen)
        //spu_streams[index].id[idlen] = 0;
        text<char_t>(id, idlen, spu_streams[index].id, idlen + 1);//ff_strncpy(spu_streams[index].id,id,idlen);// ;
    }
    spu_streams_current = index;
    DPRINTF(_l("[vobsub] subtitle (vobsubid): %d language %s"),index, spu_streams[index].id);
    return PARSE_OK;
}

Tvobsub::PARSE_RES Tvobsub::vobsub_add_altid( const char *id, size_t idlen)
{
    if (spu_streams == 0) {
        DPRINTF(_l("[vobsub] warning, binning some index entries.  Check your index file"));
        return PARSE_ERROR;
    }
    packet_queue_t *queue = spu_streams + spu_streams_current;
    if (queue->altid) free(queue->altid);
    queue->altid=(char_t*)calloc(idlen+1,sizeof(char_t));
    //memcpy(queue->altid,id,idlen);
    //queue->altid[idlen]='\0';
    text<char_t>(id, idlen, queue->altid, idlen + 1);
    return PARSE_OK;
}

Tvobsub::PARSE_RES Tvobsub::vobsub_add_timestamp( long filepos, int ms)
{
    packet_queue_t *queue;
    packet_t *pkt;
    if (spu_streams == 0) {
        DPRINTF(_l("[vobsub] warning, binning some index entries.  Check your index file"));
        return PARSE_ERROR;
    }
    queue = spu_streams + spu_streams_current;
    if (queue->packet_queue_grow() >= 0) {
        pkt = queue->packets + (queue->packets_size - 1);
        pkt->filepos = filepos;
        pkt->pts100 = ms < 0 ? UINT_MAX : (unsigned int)ms * 90;
        return PARSE_OK;
    }
    return PARSE_ERROR;
}

Tvobsub::PARSE_RES Tvobsub::vobsub_parse_id(const char *line)
{
    // id: xx, index: n
    size_t idlen;
    const char *p, *q;
    p  = line;
    while (isspace(*p))
        ++p;
    q = p;
    while (isalpha(*q))
        ++q;
    idlen = q - p;
    if (idlen == 0)
        return PARSE_ERROR;
    ++q;
    while (isspace(*q))
        ++q;
    if (strncmp("index:", q, 6))
        return PARSE_ERROR;
    q += 6;
    while (isspace(*q))
        ++q;
    if (!isdigit(*q))
        return PARSE_ERROR;
    return vobsub_add_id(p, idlen, atoi(q));
}

Tvobsub::PARSE_RES Tvobsub::vobsub_parse_altid(const char *line)
{
    // id: xx, index: n
    size_t idlen;
    const char *p, *q;
    p  = line;
    while (isspace(*p))
        ++p;
    q = p;
    while (*q && *q!='\n')
        ++q;
    idlen = q - p;
    if (idlen == 0)
        return PARSE_ERROR;
    return vobsub_add_altid(p, idlen);
}

Tvobsub::PARSE_RES Tvobsub::vobsub_parse_timestamp(const char *line)
{
    // timestamp: HH:MM:SS.mmm, filepos: 0nnnnnnnnn
    int ms;
    const char *p=parseTimestamp(line,ms);
    if (!p)
        return PARSE_ERROR;
    if (*p != ',')
        return PARSE_ERROR;
    line = p + 1;
    while (isspace(*line))
        ++line;
    if (strncmp("filepos:", line, 8))
        return PARSE_ERROR;
    line += 8;
    while (isspace(*line))
        ++line;
    if (! isxdigit(*line))
        return PARSE_ERROR;
    long filepos = strtol(line, NULL, 16);
    return vobsub_add_timestamp(filepos, delay + ms/* + 1000 * (s + 60 * (m + 60 * h))*/);
}

/* don't know how to use tridx */
Tvobsub::PARSE_RES Tvobsub::vobsub_set_lang(const char *line)
{
    if (vobsub_id == -1)
        vobsub_id = atoi(line + 8);
    return PARSE_OK;
}

Tvobsub::PARSE_RES Tvobsub::idx_parse_one_line(const char *line)
{
 if (strncmp("langidx:", line, 8) == 0)
  return vobsub_set_lang(line);
 else if (strncmp("id:", line, 3) == 0)
  return vobsub_parse_id( line + 3);
 else if (strncmp("alt:", line, 4) == 0)
  return vobsub_parse_altid( line + 4);
 else if (strncmp("timestamp:", line, 10) == 0)
  return vobsub_parse_timestamp( line + 10);
 else
  return TsubtitleDVDparent::idx_parse_one_line(line);
}

Tvobsub::Tvobsub(Tstream &fd,const char_t *const name,const char_t *const ifo,const int force,Tspudec* *spu,IffdshowBase *deci)
{
 deci->getConfig(&config);
 ok=false;
 if(spu)
   *spu = NULL;
 char_t *buf;
 spu_streams = NULL;
 spu_streams_size = 0;
 spu_streams_current = 0;
 vobsub_id=-1;
 delay = 0;
 forced_subs=0;
 buf = (char_t*)malloc((strlen(name) + 5) * sizeof(char_t));
 if (buf)
  {
   idx_parse(fd);
   /* if no palette in .idx then use custom colors */
   if ((custom_colors == 0)&&(fsppal!=1))
    custom_colors = 1;
   if (spu && rectOrig.dx && rectOrig.dy)
    *spu = new Tspudec(deci,sppal, cuspal, custom_colors, rectOrig.dx, rectOrig.dy);

   /* read the indexed mpeg_stream */
   strcpy(buf, name);
   strcat(buf, _l(".sub"));
   mpeg_t *mpg = mpeg_open(buf);
   if (mpg == NULL)
    {
     if(force)
      DPRINTF(_l("VobSub: Can't open SUB file"));
     else
      {
       free(buf);
       //free(vob);
       return /*NULL*/;
      }
    }
   else
    {
     long last_pts_diff = 0;
     while (!mpeg_eof(mpg))
      {
       long pos = mpeg_tell(mpg);
       if (mpeg_run(mpg) < 0)
        {
         if (!mpeg_eof(mpg))
          DPRINTF(_l("VobSub: mpeg_run error"));
         break;
        }
       if (mpg->packet_size)
        {
         if ((mpg->aid & 0xe0) == 0x20)
          {
           unsigned int sid = mpg->aid & 0x1f;
           if (vobsub_ensure_spu_stream(sid) >= 0)
            {
             packet_queue_t *queue = spu_streams + sid;
             //get the packet to fill
             if (queue->packets_size == 0 && queue->packet_queue_grow()  < 0)
              return;
             while (queue->current_index + 1 < queue->packets_size && queue->packets[queue->current_index + 1].filepos <= pos)
              ++queue->current_index;
             if (queue->current_index < queue->packets_size)
              {
               packet_t *pkt;
               if (queue->packets[queue->current_index].data)
                {
                 // insert a new packet and fix the PTS !
                 queue->packet_queue_insert();
                 queue->packets[queue->current_index].pts100 = mpg->pts + last_pts_diff;
                }
               pkt = queue->packets + queue->current_index;
               if (pkt->pts100 != UINT_MAX)
                {
                 if (queue->packets_size > 1)
                  last_pts_diff = pkt->pts100 - mpg->pts;
                 else
                  pkt->pts100 = mpg->pts;
                 // FIXME: should not use mpg_sub internal informations, make a copy
                 pkt->data = mpg->packet;
                 pkt->size = mpg->packet_size;
                 mpg->packet = NULL;
                 mpg->packet_reserve = 0;
                 mpg->packet_size = 0;
                }
              }
            }
          }
        }
      }
     spu_streams_current = spu_streams_size;
     while (spu_streams_current-- > 0)
      spu_streams[spu_streams_current].current_index = 0;
     mpeg_free(mpg);
    }
   free(buf);
  }
 ok=true;
}

Tvobsub::~Tvobsub()
{
    if (spu_streams) {
        while (spu_streams_size--)
            (spu_streams + spu_streams_size)->packet_queue_destroy();
        free(spu_streams);
    }
}

unsigned int Tvobsub::vobsub_get_indexes_count(void)
{
    return spu_streams_size;
}

char_t* Tvobsub::vobsub_get_id( unsigned int index)
{
    return (index < spu_streams_size) ? spu_streams[index].id : NULL;
}

unsigned int Tvobsub::vobsub_get_forced_subs_flag(void)
{
    return forced_subs;
}
/*
int Tvobsub::vobsub_set_from_lang(const char * lang)
{
    unsigned int i;
    while(lang && strlen(lang) >= 2){
      for(i=0; i < spu_streams_size; i++)
        if (spu_streams[i].id)
          if ((strncmp(spu_streams[i].id, lang, 2)==0)){
            vobsub_id=i;
            DPRINTF( _l("Selected VOBSUB language: %d language: %s"), i, text<char_t>(spu_streams[i].id));
            return 0;
          }
      lang+=2;while (lang[0]==',' || lang[0]==' ') ++lang;
    }
    DPRINTF(_l("No matching VOBSUB language found!"));
    return -1;
}
*/
int Tvobsub::vobsub_get_packet(unsigned int pts,void** data, int* timestamp) {
  unsigned int pts100 = (unsigned int)pts;//(90000 * pts);
  if (spu_streams && 0 <= vobsub_id && (unsigned) vobsub_id < spu_streams_size) {
    packet_queue_t *queue = spu_streams + vobsub_id;
    while (queue->current_index < queue->packets_size) {
      packet_t *pkt = queue->packets + queue->current_index;
      if (pkt->pts100 != UINT_MAX)
      if (pkt->pts100 <= pts100) {
        ++queue->current_index;
        *data = pkt->data;
        *timestamp = pkt->pts100;
        return pkt->size;
      } else break;
      else
          ++queue->current_index;
    }
  }
  return -1;
}

int Tvobsub::vobsub_get_next_packet( void** data, int* timestamp)
{
  if (spu_streams && 0 <= vobsub_id && (unsigned) vobsub_id < spu_streams_size) {
    packet_queue_t *queue = spu_streams + vobsub_id;
    if (queue->current_index < queue->packets_size) {
      packet_t *pkt = queue->packets + queue->current_index;
      ++queue->current_index;
      *data = pkt->data;
      *timestamp = pkt->pts100;
      return pkt->size;
    }
  }
  return -1;
}

void Tvobsub::vobsub_seek(int pts)
{
  packet_queue_t * queue;
  unsigned int seek_pts100 = (int)pts;// * 90000;

  if (spu_streams && 0 <= vobsub_id && (unsigned) vobsub_id < spu_streams_size) {
    /* do not seek if we don't know the id */
    if (vobsub_get_id(vobsub_id) == NULL)
            return;
    queue = spu_streams + vobsub_id;
    queue->current_index = 0;
    while ((queue->packets + queue->current_index)->pts100 < seek_pts100)
      ++queue->current_index;
    if (queue->current_index > 0)
      --queue->current_index;
  }
}

void Tvobsub::vobsub_reset(void)
{
    if (spu_streams) {
        unsigned int n = spu_streams_size;
        while (n-- > 0)
            spu_streams[n].current_index = 0;
    }
}
