/*
 * Copyright (C) 2020 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include "message_event.h"

#include "media_error_event.h"

namespace kraken::binding::jsc {

void bindMessageEvent(std::unique_ptr<JSContext> &context) {
  auto event = JSMessageEvent::instance(context.get());
  JSC_GLOBAL_SET_PROPERTY(context, "MessageEvent", event->classObject);
};

std::unordered_map<JSContext *, JSMessageEvent *> JSMessageEvent::getInstanceMap() {
  static std::unordered_map<JSContext *, JSMessageEvent *> instanceMap;
  return instanceMap;
}

JSMessageEvent *JSMessageEvent::instance(JSContext *context) {
  auto instanceMap = getInstanceMap();
  if (instanceMap.count(context) == 0) {
    instanceMap[context] = new JSMessageEvent(context);
  }
  return instanceMap[context];
}

JSMessageEvent::~JSMessageEvent() {
  auto instanceMap = getInstanceMap();
  instanceMap.erase(context);
}

JSMessageEvent::JSMessageEvent(JSContext *context) : JSEvent(context, "MessageEvent") {}

JSObjectRef JSMessageEvent::instanceConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                                const JSValueRef *arguments, JSValueRef *exception) {
  if (argumentCount != 1) {
    JSC_THROW_ERROR(ctx, "Failed to construct 'JSMessageEvent': 1 argument required, but only 0 present.", exception);
    return nullptr;
  }

  JSStringRef dataStringRef = JSValueToStringCopy(ctx, arguments[0], exception);
  auto event = new MessageEventInstance(this, dataStringRef);
  return event->object;
}

JSValueRef JSMessageEvent::getProperty(std::string &name, JSValueRef *exception) {
  return nullptr;
}

MessageEventInstance::MessageEventInstance(JSMessageEvent *jsMessageEvent, NativeMessageEvent *nativeMessageEvent)
  : EventInstance(jsMessageEvent, nativeMessageEvent->nativeEvent), nativeMessageEvent(nativeMessageEvent) {
  if (nativeMessageEvent->data != nullptr) m_data.setString(nativeMessageEvent->data);
  if (nativeMessageEvent->origin != nullptr) m_origin.setString(nativeMessageEvent->origin);
}

MessageEventInstance::MessageEventInstance(JSMessageEvent *jsMessageEvent, JSStringRef data)
  : EventInstance(jsMessageEvent, "message", nullptr, nullptr) {
  nativeMessageEvent = new NativeMessageEvent(nativeEvent);
}

JSValueRef MessageEventInstance::getProperty(std::string &name, JSValueRef *exception) {
  auto propertyMap = JSMessageEvent::getMessageEventPropertyMap();

  if (propertyMap.count(name) == 0) return EventInstance::getProperty(name, exception);

  auto property = propertyMap[name];

  switch(property) {
  case JSMessageEvent::MessageEventProperty::kData:
    return m_data.makeString();
  case JSMessageEvent::MessageEventProperty::kOrigin:
    return m_origin.makeString();
  }

  return nullptr;
}

void MessageEventInstance::setProperty(std::string &name, JSValueRef value, JSValueRef *exception) {
  auto propertyMap = JSMessageEvent::getMessageEventPropertyMap();
  if (propertyMap.count(name) > 0) {
    auto property = propertyMap[name];

    switch(property) {
    case JSMessageEvent::MessageEventProperty::kData: {
      JSStringRef str = JSValueToStringCopy(ctx, value, exception);
      m_data.setString(str);
      break;
    }
    case JSMessageEvent::MessageEventProperty::kOrigin: {
      JSStringRef str = JSValueToStringCopy(ctx, value, exception);
      m_origin.setString(str);
      break;
    }
    }
  } else {
    EventInstance::setProperty(name, value, exception);
  }
}

MessageEventInstance::~MessageEventInstance() {
  nativeMessageEvent->data->free();
  nativeMessageEvent->origin->free();
  delete nativeMessageEvent;
}

void MessageEventInstance::getPropertyNames(JSPropertyNameAccumulatorRef accumulator) {
  EventInstance::getPropertyNames(accumulator);

  for (auto &property : JSMessageEvent::getMessageEventPropertyNames()) {
    JSPropertyNameAccumulatorAddName(accumulator, property);
  }
}

std::vector<JSStringRef> &JSMessageEvent::getMessageEventPropertyNames() {
  static std::vector<JSStringRef> propertyNames{JSStringCreateWithUTF8CString("data"),
                                                JSStringCreateWithUTF8CString("origin")};
  return propertyNames;
}

const std::unordered_map<std::string, JSMessageEvent::MessageEventProperty> &
JSMessageEvent::getMessageEventPropertyMap() {
  static std::unordered_map<std::string, MessageEventProperty> propertyMap{{"data", MessageEventProperty::kData},
                                                                           {"origin", MessageEventProperty::kOrigin}};
  return propertyMap;
}

} // namespace kraken::binding::jsc
