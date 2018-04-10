#include "DBLink.hh"
namespace COSMIC{

DBLink::DBLink(){
  this->db = NULL;
  this->dbtable = NULL;
  this->tblname = "";
  this->index = "";
  this->fLocked = true;
}

DBLink::DBLink(DB *dbs, DBTable* dbt, std::string tbl, std::string ind){
  this->db = dbs;
  this->dbtable = dbt;
  this->tblname = tbl;
  this->index = ind;
  this->fLocked = true;
}

DBLink::~DBLink(){
  this->db = 0;
  this->dbtable = 0; // don't remove pointer
  this->tblname = "";
  this->index = "";
}
std::vector<std::string> DBLink::GetFields(){
  return dbtable->GetFields();
}

std::string DBLink::GetTableName(){
  return dbtable->GetTableName();
}

std::string DBLink::GetIndexName(){
  return dbtable->GetIndexName();
}

bool DBLink::Has(std::string name){
  if (!dbtable) throw;
  return dbtable->Has(name);
}

bool DBLink::GetB(std::string name){
  if (!dbtable) throw;
  return dbtable->GetI(name) > 0;
}

int DBLink::GetI(std::string name){
  if (!dbtable) throw;
  return dbtable->GetI(name);
}

float DBLink::GetF(std::string name){
  if (!dbtable) throw;
  return dbtable->GetF(name);
}

double DBLink::GetD(std::string name){
  if (!dbtable) throw;
  return dbtable->GetD(name);
}

std::string DBLink::GetS(std::string name){
  if (!dbtable) throw;
  return dbtable->GetS(name);
}


std::vector<int> DBLink::GetVecI(std::string name){
  if (!dbtable) throw;
  return dbtable->GetVecI(name);
}

std::vector<float> DBLink::GetVecF(std::string name){
  if (!dbtable) throw;
  return dbtable->GetVecF(name);
}

std::vector<double> DBLink::GetVecD(std::string name){
  if (!dbtable) throw;
  return dbtable->GetVecD(name);
}

std::vector<std::string> DBLink::GetVecS(std::string name){
  if (!dbtable) throw;
  return dbtable->GetVecS(name);
}

void DBLink::Unlock(){
  fLocked = false;
}

void DBLink::Lock(){
  fLocked = true;
}

void DBLink::Print(){
  std::cout << "DBLink : " << std::endl;
  dbtable->Print();
}


}