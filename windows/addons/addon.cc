#include <node_api.h>
#include <assert.h>

napi_value HelloWorld(napi_env env, napi_callback_info info) {
  napi_status status;

  // Створення рядка "Hello, World!" для повернення
  napi_value result;
  status = napi_create_string_utf8(env, "Hello, World!", NAPI_AUTO_LENGTH, &result);
  assert(status == napi_ok);

  return result;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;

  // Створення функції "helloWorld" і прив'язка її до модуля
  napi_value hello_world_fn;
  status = napi_create_function(env, NULL, 0, HelloWorld, NULL, &hello_world_fn);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "helloWorld", hello_world_fn);
  assert(status == napi_ok);

  return exports;
}

// Ініціалізація модуля
NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
