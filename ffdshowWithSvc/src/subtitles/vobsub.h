#ifndef _VOBSUB_H_
#define _VOBSUB_H_

#include "TsubtitleDVD.h"
#include "interfaces.h"

class Tspudec;
struct Tconfig;
struct Tstream;
class Tvobsub :public TsubtitleDVDparent
{
private:
 const Tconfig *config;
 Tstream* rar_open(const char_t *flnm);
public:
 struct packet_t
  {
   unsigned int pts100;
   long filepos;
   unsigned int size;
   unsigned char *data;
   void packet_construct(void);
   void packet_destroy(void);
  };
 /* index */
 struct packet_queue_t
  {
   char_t *id;
   char_t *altid;
   packet_t *packets;
   unsigned int packets_reserve;
   unsigned int packets_size;
   unsigned int current_index;
   void packet_queue_construct(void);
   void packet_queue_destroy(void);
   int packet_queue_ensure(unsigned int needed_size);
   int packet_queue_grow(void);
   int packet_queue_insert(void);
  };
 packet_queue_t *spu_streams;
 unsigned int spu_streams_size;
 unsigned int spu_streams_current;
private:
 struct mpeg_t
  {
   Tstream *stream;
   unsigned int pts;
   int aid;
   unsigned char *packet;
   unsigned int packet_reserve;
   unsigned int packet_size;
  };
 mpeg_t *mpeg_open(const char_t *filename);
 static void mpeg_free(mpeg_t *mpeg);
 static int mpeg_eof(mpeg_t *mpeg);
 static long mpeg_tell(mpeg_t *mpeg);
 static int mpeg_run(mpeg_t *mpeg);

 int vobsub_ensure_spu_stream(unsigned int index);
 PARSE_RES vobsub_set_lang(const char *line);
 PARSE_RES vobsub_add_id( const char *id, size_t idlen, const unsigned int index);
 PARSE_RES vobsub_add_altid( const char *id, size_t idlen);
 PARSE_RES vobsub_add_timestamp( long filepos, int ms);
 PARSE_RES vobsub_parse_id(const char *line);
 PARSE_RES vobsub_parse_altid(const char *line);
 PARSE_RES vobsub_parse_timestamp(const char *line);
 virtual PARSE_RES idx_parse_one_line(const char *line);
public:
 bool ok;

 int vobsub_id;

 Tvobsub(Tstream &f,const char_t *flnm2, const char_t *const ifo, const int force, Tspudec* *spu,IffdshowBase *deci);
 void vobsub_reset(void);
 int vobsub_parse_ifo(const char *const name, unsigned int *palette, unsigned int *width, unsigned int *height, int force, int sid, char *langid);
 int vobsub_get_packet(unsigned int pts,void** data, int* timestamp);
 int vobsub_get_next_packet(void** data, int* timestamp);
 ~Tvobsub();
 unsigned int vobsub_get_indexes_count(void);
 char_t *vobsub_get_id(unsigned int /* index */);

 //int vobsub_set_from_lang(const char * lang);
 unsigned int vobsub_get_forced_subs_flag(void);
 void vobsub_seek(int pts);
};

#endif





