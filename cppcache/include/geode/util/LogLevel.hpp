//
// Created by Jacob Barrett on 12/6/17.
//

#pragma once

#ifndef GEODE_UTIL_LOGLEVEL_HPP
#define GEODE_UTIL_LOGLEVEL_HPP

namespace apache {
namespace geode {
namespace client {

enum LogLevel {

  // NOTE: if you change this enum declaration at all, be sure to
  // change levelToChars and charsToLevel functions!

  None,

  Error,
  Warning,
  Info,

  Default,

  Config,

  Fine,
  Finer,
  Finest,

  Debug,

  All

};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif //GEODE_UTIL_LOGLEVEL_HPP

