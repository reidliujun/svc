/*
 * Copyright (c) 2005,2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Toptions.h"
#include "IffdshowParamsEnum.h"
#include "reg.h"

//======================================== Toptions ========================================
Toptions::Toptions(TintStrColl *Icoll):coll(Icoll)
{
}
Toptions::~Toptions()
{
 if (coll) coll->removeColl(this);
}
void Toptions::addOptions(const TintOption *iopts)
{
 if (coll) coll->addOptions(iopts,this);
}
void Toptions::addOptions(const TstrOption *sopts)
{
 if (coll) coll->addOptions(sopts,this);
}
void Toptions::reg_op(TregOp &t)
{
 if (coll) coll->reg_op(t,this);
}
void Toptions::resetValues(const int *ids)
{
 if (coll) coll->reset(ids,this);
}
void Toptions::setParamList(int id,const TcreateParamListBase *createParamList)
{
 if (coll) coll->setParamList(id,createParamList);
}

template<class Topt> void Toptions::getInfo(const Topt *self,TffdshowParamInfo *info)
{
 info->valnamePtr=self->regname?self->regname:_l("?");if (info->valname) text<char>(info->valnamePtr,info->valname);
 info->namePtr=self->name;if (info->name) text<char>(info->namePtr,info->name);
 info->id=self->id;
 info->inPreset=self->inPreset;
 info->ptr=(void*)&(this->*(self->val));
}
bool Toptions::notifyChange(int id,int val)
{
 return coll?coll->notifyParam(id,val):false;
}
bool Toptions::notifyChange(int id,const char_t *val)
{
 return coll?coll->notifyParam(id,val):false;
}

//================================= Toptions::TintOption ===================================
bool Toptions::TintOption::get(Toptions *self,int *valPtr) const
{
 if (!val || (min==-2 && max==-2)) return false;
 *valPtr=self->*val;
 return true;
}
void Toptions::TintOption::set(Toptions *self,int newVal,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const
{
 int min,max;
 if (this->min==-3 && this->max==-3)
  self->getMinMax(id,min,max);
 else
  {min=this->min;max=this->max;}
 if (min==max && min<0) return;
 int oldVal=self->*val;
 if (min!=max)
  newVal=limit(newVal,min,max);
 self->*val=newVal;
 if (oldVal!=newVal)
  {
   if (!sendOnChange.empty()) sendOnChange(id,newVal);
   if (!onChangeBind.empty()) onChangeBind(id,newVal);
  }
}
bool Toptions::TintOption::inv(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const
{
 if (min==0 && max==0)
  {
   int oldval;
   get(self,&oldval);
   set(self,1-oldval,sendOnChange,onChangeBind);
   return true;
  }
 else
  return false;
}

void Toptions::TintOption::getInfo(Toptions *self,TffdshowParamInfo *info) const
{
 self->getInfo(this,info);
 info->type=FFDSHOW_PARAM_INT;
 info->min=min;info->max=max;
}
void Toptions::TintOption::reg_op(Toptions *self,TregOp &t) const
{
 if (regname)
  t._REG_OP_N(short(id),regname,self->*val,def==DEF_DYN?self->getDefault(id):def);
}
void Toptions::TintOption::reset(Toptions *self) const
{
 self->*val=(def==DEF_DYN?self->getDefault(id):def);
}
void Toptions::TintOption::reset(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const
{
 set(self,def==DEF_DYN?self->getDefault(id):def,sendOnChange,onChangeBind);
}

//================================= Toptions::TstrOption ===================================
bool Toptions::TstrOption::get(Toptions *self,const char_t* *valPtr) const
{
 *valPtr=&(self->*val);
 return true;
}
void Toptions::TstrOption::set(Toptions *self,const char_t *newval,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const
{
 if (buflen==0) return;
 char_t *val=&(self->*(this->val));
 int dif=strcmp(newval,val);
 ff_strncpy(val,newval,buflen);
 val[buflen-1]='\0';
 if (dif!=0)
  {
   if (!sendOnChange.empty()) sendOnChange(id,0);
   if (!onChangeBind.empty()) onChangeBind(id,newval);
  }
}

void Toptions::TstrOption::getInfo(Toptions *self,TffdshowParamInfo *info) const
{
 self->getInfo(this,info);
 info->type=FFDSHOW_PARAM_STRING;
 info->maxlen=(int)buflen;
}

void Toptions::TstrOption::reg_op(Toptions *self,TregOp &t) const
{
 if (regname)
  {
   if (def)
    t._REG_OP_S(short(id), regname, &(self->*val), buflen, def, multipleLines);
   else
    {
     char_t *defbuf=(char_t*)_alloca((buflen+1)*sizeof(char_t));defbuf[0]='\0';
     self->getDefaultStr(id,defbuf,buflen);
     t._REG_OP_S(short(id), regname, &(self->*val), buflen, defbuf, multipleLines);
    }
  }
}
void Toptions::TstrOption::reset(Toptions *self) const
{
 if (def)
  strcpy(&(self->*val),def);
 else
  self->getDefaultStr(id,&(self->*val),buflen);
}
void Toptions::TstrOption::reset(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const
{
 if (def)
  set(self,def,sendOnChange,onChangeBind);
 else
  {
   char_t *defbuf=(char_t*)_alloca((buflen+1)*sizeof(char_t));defbuf[0]='\0';
   self->getDefaultStr(id,defbuf,buflen);
   set(self,defbuf,sendOnChange,onChangeBind);
  }
}

//===================================== TcollOptions =======================================
template<class Toption,class Tbase> bool TcollOptions<Toption,Tbase>::getInfo(int id,TffdshowParamInfo *info)
{
 typename Tbase::const_iterator o=this->find(id);
 if (o==this->end())
  return false;
 else
  {
   o->second.option->getInfo(o->second.coll,info);
   info->isNotify=!o->second.onChange.empty();
   return true;
  }
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::getInfoIDs(ints &infos)
{
 for (typename Tbase::const_iterator o=this->begin();o!=this->end();o++)
  infos.push_back(o->second.option->id);
}
template<class Toption,class Tbase> bool TcollOptions<Toption,Tbase>::notify(int id,Tval val)
{
 typename Tbase::const_iterator o=this->find(id);
 return o!=this->end() && !o->second.onChange.empty()?(o->second.onChange(id,val),true):false;
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::addOptions(const Toption *opts,Toptions *coll)
{
 for (;opts && opts->id;opts++)
  {
   typename Tbase::iterator o=find(opts->id);
   if (o==this->end())
    insert(std::make_pair(opts->id,TcollOption<Toption>(coll,opts)));
   else
    o->second=TcollOption<Toption>(coll,opts);
  }
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::removeColl(Toptions *coll)
{
 ints ii;
 for (typename Tbase::iterator i=this->begin();i!=this->end();i++)
  if (i->second.coll==coll)
   ii.push_back(i->first);
 for (ints::iterator i=ii.begin();i!=ii.end();i++)
  this->erase(*i);
/*
 for (typename Tbase::iterator i=this->begin();i!=this->end();i++)
  if (i->second.coll==coll)
   i=erase(i);
*/
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::setOnChange(int id,const typename Toption::TonChangeBind &bind)
{
 typename Tbase::iterator o=this->find(id);
 if (o!=this->end())
  o->second.onChange=bind;
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::notifyParamsChanged(void)
{
 typedef std::vector<const typename Toption::TonChangeBind*> Tbinds;
 Tbinds binds;
 for (typename Tbase::iterator o=this->begin();o!=this->end();o++)
  if (!o->second.onChange.empty() && std::find_if(binds.begin(),binds.end(),TonChangeCmp<const typename Toption::TonChangeBind>(o->second.onChange))==binds.end())
   binds.push_back(&o->second.onChange);
 for (typename Tbinds::iterator b=binds.begin();b!=binds.end();b++)
  (**b)(0,0);
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::reg_op(TregOp &t,Toptions *coll)
{
 for (typename Tbase::iterator o=this->begin();o!=this->end();o++)
  if (o->second.coll==coll)
   o->second.option->reg_op(coll,t);
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::reset(const int *idBegin,const int *idEnd,Toptions *coll)
{
 for (typename Tbase::iterator o=this->begin();o!=this->end();o++)
  if (std::find(idBegin,idEnd,o->first)!=idEnd)
   o->second.option->reset(coll);
}
template<class Toption,class Tbase> void TcollOptions<Toption,Tbase>::setParamList(int id,const TcreateParamListBase *createParamList)
{
 typename Tbase::iterator o=this->find(id);
 if (o!=this->end())
  o->second.createParamList=createParamList;
}
template<class Toption,class Tbase> TparamListItems* TcollOptions<Toption,Tbase>::getParamList(int id)
{
 typename Tbase::const_iterator o=this->find(id);
 return o==this->end() || !o->second.createParamList?NULL:o->second.createParamList->create();
}

//===================================== TintStrColl ========================================
void TintStrColl::addOptions(const Toptions::TintOption *iopts,Toptions *coll)
{
 first.addOptions(iopts,coll);
}
void TintStrColl::addOptions(const Toptions::TstrOption *sopts,Toptions *coll)
{
 second.addOptions(sopts,coll);
}
void TintStrColl::removeColl(Toptions *coll)
{
 first.removeColl(coll);
 second.removeColl(coll);
 if (first.empty() && second.empty())
  delete this;
}
void TintStrColl::setOnChange(int id,const Toptions::TintOption::TonChangeBind &bind)
{
 first.setOnChange(id,bind);
}
void TintStrColl::setOnChange(int id,const Toptions::TstrOption::TonChangeBind &bind)
{
 second.setOnChange(id,bind);
}
void TintStrColl::setSendOnChange(const Toptions::TsendOnChange &IsendOnChange)
{
 sendOnChange=IsendOnChange;
}
void TintStrColl::notifyParamsChanged(void)
{
 first.notifyParamsChanged();
 second.notifyParamsChanged();
}
bool TintStrColl::notifyParam(int id,int val)
{
 return first.notify(id,val);
}
bool TintStrColl::notifyParam(int id,const char_t *val)
{
 return second.notify(id,val);
}
bool TintStrColl::getInfo(int id,TffdshowParamInfo *info)
{
 return first.getInfo(id,info) || second.getInfo(id,info);
}
void TintStrColl::reg_op(TregOp &t,Toptions *coll)
{
 first.reg_op(t,coll);
 second.reg_op(t,coll);
}
void TintStrColl::reset(const int *ids,Toptions *coll)
{
 const int *end=ids;
 while (*end) end++;
 first.reset(ids,end,coll);
 second.reset(ids,end,coll);
}
bool TintStrColl::reset(int id)
{
 return first.reset(id,sendOnChange) || second.reset(id,sendOnChange);
}
void TintStrColl::getInfoIDs(ints &infos)
{
 first.getInfoIDs(infos);
 second.getInfoIDs(infos);
}
void TintStrColl::setParamList(int id,const TcreateParamListBase *createParamList)
{
 first.setParamList(id,createParamList);
 second.setParamList(id,createParamList);
}
TparamListItems* TintStrColl::getParamList(int id)
{
 TparamListItems *list;
 if ((list=first.getParamList(id))!=NULL) return list;
 else if ((list=second.getParamList(id))!=NULL) return list;
 else return NULL;
}
