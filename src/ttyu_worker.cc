#include <ttyu.h>

void ttyu_worker_c::ttyu_progress_c::send(const ttyu_event_t *event) const {
  that_->send_(event);
}

ttyu_worker_c::ttyu_progress_c::ttyu_progress_c(ttyu_worker_c *that) :
    that_(that) {}

ttyu_worker_c::ttyu_worker_c(ttyu_js_c *obj) :
    NanAsyncWorker(NULL), asyncdata_(NULL), obj_(obj) {
  async = new uv_async_t;
  async_lock = new uv_mutex_t;
  uv_mutex_init(async_lock);
  uv_async_init(uv_default_loop(), async, async_progress_);
  async->data = this;
}

ttyu_worker_c::~ttyu_worker_c() {
  uv_mutex_destroy(async_lock);
  ttyu_event_destroy(asyncdata_);
  free(asyncdata_);
}

void ttyu_worker_c::progress() {
  uv_mutex_lock(async_lock);
  ttyu_event_t *event = asyncdata_;
  asyncdata_ = NULL;
  uv_mutex_unlock(async_lock);

  if(event) {
    handle(event);
  }
  ttyu_event_destroy(event);
  free(event);
}

void ttyu_worker_c::handle(ttyu_event_t *event) {
  if(ee_count(obj_->emitter, event->type) == 0) {
    return;
  }
  v8::Local<v8::Value> arg;
  v8::Local<v8::Object> obj;
  switch(event->type) {
    case EVENT_RESIZE:
      arg = NanUndefined();
      break;
    case EVENT_KEY:
      obj = NanNew<v8::Object>();
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->key->ctrl));
      obj->Set(NanNew<v8::String>("char"), NanNew<v8::String>(&event->key->c));
      obj->Set(NanNew<v8::String>("code"),
          NanNew<v8::Integer>(event->key->code));
      obj->Set(NanNew<v8::String>("which"),
          NanNew<v8::Integer>(event->key->which));
      arg = obj;
      break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE:
    case EVENT_MOUSEWHEEL:
    case EVENT_MOUSEHWHEEL:
      obj = NanNew<v8::Object>();
      obj->Set(NanNew<v8::String>("button"),
          NanNew<v8::Integer>(event->mouse->button));
      obj->Set(NanNew<v8::String>("x"), NanNew<v8::Integer>(event->mouse->x));
      obj->Set(NanNew<v8::String>("y"), NanNew<v8::Integer>(event->mouse->y));
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->mouse->ctrl));
      arg = obj;
      break;
    default: // EVENT_ERROR
      arg = NanError(event->err);
      break;
  }
  ee_emit(obj_->emitter, event->type, arg);
}

void ttyu_worker_c::destroy() {
  uv_close(reinterpret_cast<uv_handle_t*>(async), async_close_);
}

void ttyu_worker_c::Execute() {
  ttyu_progress_c progress(this);
  // loop execute until it returns false (error)
  while(execute(progress, obj_->data));
}

void ttyu_worker_c::send_(const ttyu_event_t *event) {
  ttyu_event_t *new_event = (ttyu_event_t *)malloc(sizeof(event));
  memcpy(&new_event, &event, sizeof(event));
  uv_mutex_lock(async_lock);
  ttyu_event_t *old_event = asyncdata_;
  asyncdata_ = new_event;
  uv_mutex_unlock(async_lock);

  ttyu_event_destroy(old_event);
  free(old_event);
  uv_async_send(async);
}

NAUV_WORK_CB(ttyu_worker_c::async_progress_) {
  ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(async->data);
  worker->progress();
}

void ttyu_worker_c::async_close_(uv_handle_t *handle) {
  ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(handle->data);
  delete reinterpret_cast<uv_async_t*>(handle);
  delete worker;
}