/*
 * Copyright (C) 2019-present Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

import 'dart:ui';
import 'dart:convert';
import 'package:kraken/bridge.dart';
import 'package:kraken/dom.dart';

const String WINDOW = 'WINDOW';

class Window extends EventTarget {
  Window(ElementManager elementManager) : super(WINDOW_ID, elementManager) {
    window.onPlatformBrightnessChanged = () {
      Event event = Event('colorschemechange');
      event.detail = (window.platformBrightness == Brightness.light) ? 'light' : 'dart';
      dispatchEvent(event);
    };
  }

  void _handleColorSchemeChange(Event event) {
    String json = jsonEncode([targetId, event]);
    emitUIEvent(elementManager.controller.view.contextId, json);
  }

  void _handleLoad(Event event) {
    String json = jsonEncode([targetId, event]);
    emitUIEvent(elementManager.controller.view.contextId, json);
  }

  @override
  void addEvent(String eventName) {
    super.addEvent(eventName);
    if (eventHandlers.containsKey(eventName)) return; // Only listen once.

    switch (eventName) {
      case 'colorschemechange':
        return addEventListener(eventName, _handleColorSchemeChange);
      case 'load':
        return addEventListener(eventName, _handleLoad);
    }
  }
}