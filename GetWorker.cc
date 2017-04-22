#include <nan.h>
#include <iostream>
#include "GetWorker.h"  
#include "rocksdb/db.h"

GetWorker::GetWorker(Nan::Callback *callback, rocksdb::DB *db, rocksdb::Slice key, bool buffer)
    : AsyncWorker(callback), _db(db), _key(key), _buffer(buffer) {}
GetWorker::~GetWorker() {}

void GetWorker::Execute () {
  _status = _db->Get(rocksdb::ReadOptions(), _key, &_value);
}

void GetWorker::HandleOKCallback () {
  Nan::HandleScope scope;
  
  v8::Local<v8::Value> argv[2] = { Nan::Null(), Nan::Null() };
  if (!_status.ok()) {
    v8::Local<v8::Value> errv[1] = {};
    errv[0] = Nan::New<v8::String>(_status.getState()).ToLocalChecked();
    callback->Call(1, errv);
    return;
  } 

  if (_status.IsNotFound()) {
    callback->Call(2, argv);
    return;
  }

  if (_buffer) {
    argv[1] = Nan::CopyBuffer((char*)_value.data(), _value.size()).ToLocalChecked();
  } else {
    argv[1] = Nan::New<v8::String>((char*)_value.data()).ToLocalChecked();
  }
  callback->Call(2, argv);
}
