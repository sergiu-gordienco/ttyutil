/* ttyutil - unix/start.cc
 * https://github.com/clidejs/ttyutil
 *
 * Copyright Bernhard Bücherl <bernhard.buecherl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <ttyu.h>

NAN_METHOD(ttyu_js_c::js_start) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->running = TRUE;
  obj->stop = FALSE;
  obj->worker_run = TRUE;
  obj->win = initscr();
  obj->x = getcurx(obj->win);
  obj->y = getcury(obj->win);

  uv_barrier_init(&obj->barrier, 3);
  uv_mutex_init(&obj->emitstacklock);
  uv_mutex_init(&obj->ungetlock);
  uv_cond_init(&obj->condition);

  uv_thread_create(&obj->curses_thread, ttyu_js_c::curses_thread_func, obj);
  obj->check_queue();

  uv_barrier_wait(&obj->barrier);
  NanReturnUndefined();
}
