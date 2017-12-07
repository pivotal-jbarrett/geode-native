/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <geode/geode_globals.hpp>

#include <atomic>

#include <ace/OS_NS_stdio.h>
#include "AtomicStatisticsImpl.hpp"
#include "StatisticsTypeImpl.hpp"
#include "StatisticDescriptorImpl.hpp"
#include "../Assert.hpp"


namespace apache {
namespace geode {
namespace statistics {
/**
 * An implementation of {@link Statistics} that stores its statistics
 * in local  memory and supports atomic operations.
 *
 */
//////////////////////  Static Methods  //////////////////////

int64_t AtomicStatisticsImpl::calcNumericId(StatisticsFactory* system,
                                            int64_t userValue) {
  int64_t result;
  if (userValue != 0) {
    result = userValue;
  } else {
    result = system->getId();
  }
  return result;
}

std::string AtomicStatisticsImpl::calcTextId(StatisticsFactory* system,
                                             const std::string& userValue) {
  if (!userValue.empty()) {
    return userValue;
  } else {
    if (system != nullptr) {
      return system->getName();
    } else {
      return "";
    }
  }
}

///////////////////////  Constructors  ///////////////////////

/**
 * Creates a new statistics instance of the given type
 *
 * @param type
 *        A description of the statistics
 * @param textId
 *        Text that identifies this statistic when it is monitored
 * @param numericId
 *        A number that displayed when this statistic is monitored
 * @param uniqueId
 *        A number that uniquely identifies this instance
 * @param system
 *        The distributed system that determines whether or not these
 *        statistics are stored (and collected) in local memory
 */
AtomicStatisticsImpl::AtomicStatisticsImpl(StatisticsType* typeArg,
                                           const std::string& textIdArg,
                                           int64_t numericIdArg,
                                           int64_t uniqueIdArg,
                                           StatisticsFactory* system)

{
  try {
    this->textIdStr = calcTextId(system, textIdArg);
    this->numericId = calcNumericId(system, numericIdArg);
    this->uniqueId = uniqueIdArg;
    this->closed = false;
    this->statsType = dynamic_cast<StatisticsTypeImpl*>(typeArg);
    GF_D_ASSERT(this->statsType != nullptr);
    int32_t intCount = statsType->getIntStatCount();
    int32_t longCount = statsType->getLongStatCount();
    int32_t doubleCount = statsType->getDoubleStatCount();

    if (intCount > 0) {
      intStorage = new std::atomic<int32_t>[intCount];
      for (int32_t i = 0; i < intCount; i++) {
        intStorage[i] = 0;  // Un-initialized state
      }

    } else {
      intStorage = nullptr;
    }
    if (longCount > 0) {
      longStorage = new std::atomic<int64_t>[longCount];
      for (int32_t i = 0; i < longCount; i++) {
        longStorage[i] = 0;  // Un-initialized state
      }

    } else {
      longStorage = nullptr;
    }
    if (doubleCount > 0) {
      doubleStorage = new std::atomic<double>[doubleCount];
      for (int32_t i = 0; i < doubleCount; i++) {
        doubleStorage[i] = 0;  // Un-initialized state
      }
    } else {
      doubleStorage = nullptr;
    }
  } catch (...) {
    statsType = nullptr;  // Will be deleted by the class who calls this ctor
  }
}

AtomicStatisticsImpl::~AtomicStatisticsImpl() {
  try {
    statsType = nullptr;
    if (intStorage != nullptr) {
      delete[] intStorage;
      intStorage = nullptr;
    }
    if (longStorage != nullptr) {
      delete[] longStorage;
      longStorage = nullptr;
    }
    if (doubleStorage != nullptr) {
      delete[] doubleStorage;
      doubleStorage = nullptr;
    }
  } catch (...) {
  }
}

//////////////////////  Instance Methods  //////////////////////

bool AtomicStatisticsImpl::isShared() const { return false; }

bool AtomicStatisticsImpl::isAtomic() const {
  return true;  // will always be true for this class
}

void AtomicStatisticsImpl::close() {
  // Just mark closed,Will be actually deleted when token written in archive
  // file.
  closed = true;
}

////////////////////////  store() Methods  ///////////////////////

void AtomicStatisticsImpl::_setInt(int32_t offset, int32_t value) {
  if (offset >= statsType->getIntStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128, "setInt:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }
  intStorage[offset] = value;
}

void AtomicStatisticsImpl::_setLong(int32_t offset, int64_t value) {
  if (offset >= statsType->getLongStatCount()) {
    char s[128] = {'\0'};

    ACE_OS::snprintf(
        s, 128, "setLong:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  longStorage[offset] = value;
}

void AtomicStatisticsImpl::_setDouble(int32_t offset, double value) {
  if (offset >= statsType->getDoubleStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128,
        "setDouble:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  doubleStorage[offset] = value;
}

///////////////////////  get() Methods  ///////////////////////

int32_t AtomicStatisticsImpl::_getInt(int32_t offset) const {
  if (offset >= statsType->getIntStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128, "getInt:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  return intStorage[offset];
}

int64_t AtomicStatisticsImpl::_getLong(int32_t offset) const {
  if (offset >= statsType->getLongStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128, "getLong:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }
  return longStorage[offset];
}

double AtomicStatisticsImpl::_getDouble(int32_t offset) const {
  if (offset >= statsType->getDoubleStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128,
        "getDouble:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }
  return doubleStorage[offset];
}

int64_t AtomicStatisticsImpl::_getRawBits(
    const StatisticDescriptor* statDscp) const {
  const auto stat = dynamic_cast<const StatisticDescriptorImpl*>(statDscp);
  switch (stat->getTypeCode()) {
    case INT_TYPE:
      return getInt(stat->getId());

    case LONG_TYPE:
      return _getLong(stat->getId());

    case DOUBLE_TYPE: {
      double value = _getDouble(stat->getId());
      int64_t* temp = reinterpret_cast<int64_t*>(&value);
      return *temp;
    }
    default:
      return 0;
      /*throw RuntimeException("unexpected stat descriptor type code: " +
                                stat->getTypeCode());*/
  }
}

int64_t AtomicStatisticsImpl::getRawBits(
    const StatisticDescriptor* descriptor) const {
  if (isOpen()) {
    return _getRawBits(descriptor);
  } else {
    return 0;
  }
}

////////////////////////  inc() Methods  ////////////////////////

int32_t AtomicStatisticsImpl::_incInt(int32_t offset, int32_t delta) {
  if (offset >= statsType->getIntStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128, "incInt:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  return (intStorage[offset] += delta);
}

int64_t AtomicStatisticsImpl::_incLong(int32_t offset, int64_t delta) {
  if (offset >= statsType->getLongStatCount()) {
    char s[128] = {'\0'};
    /* adongre  - Coverity II
     * CID 29273: Calling risky function (SECURE_CODING)[VERY RISKY]. Using
     * "sprintf" can cause a
     * buffer overflow when done incorrectly. Because sprintf() assumes an
     * arbitrarily long string,
     * callers must be careful not to overflow the actual space of the
     * destination.
     * Use snprintf() instead, or correct precision specifiers.
     * Fix : using ACE_OS::snprintf
     */
    // sprintf(s, "incLong:The id (%d) of the Statistic Descriptor is not valid
    // ", offset);

    ACE_OS::snprintf(
        s, 128, "incLong:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  return (longStorage[offset] += delta);
}

double AtomicStatisticsImpl::_incDouble(int32_t offset, double delta) {
  if (offset >= statsType->getDoubleStatCount()) {
    char s[128] = {'\0'};
    ACE_OS::snprintf(
        s, 128,
        "incDouble:The id (%d) of the Statistic Descriptor is not valid ",
        offset);
    throw IllegalArgumentException(s);
  }

  double expected = doubleStorage[offset];
  double value;
  do {
    value = expected + delta;
  } while (!doubleStorage[offset].compare_exchange_weak(expected, value));

  return value;
}

/**************************Base class methods ********************/

//////////////////////  Instance Methods  //////////////////////

int32_t AtomicStatisticsImpl::nameToId(const std::string& name) const {
  return statsType->nameToId(name);
}

StatisticDescriptor* AtomicStatisticsImpl::nameToDescriptor(
    const std::string& name) const {
  return statsType->nameToDescriptor(name);
}

bool AtomicStatisticsImpl::isClosed() const { return closed; }

bool AtomicStatisticsImpl::isOpen() const { return !closed; }

////////////////////////  attribute Methods  ///////////////////////

StatisticsType* AtomicStatisticsImpl::getType() const { return statsType; }

const std::string& AtomicStatisticsImpl::getTextId() const { return textIdStr; }

int64_t AtomicStatisticsImpl::getNumericId() const { return numericId; }

/**
 * Gets the unique id for this resource
 */
int64_t AtomicStatisticsImpl::getUniqueId() const { return uniqueId; }

////////////////////////  set() Methods  ///////////////////////

void AtomicStatisticsImpl::setInt(const std::string& name, int32_t value) {
  int32_t id = getIntId(nameToDescriptor(name));
  setInt(id, value);
}

void AtomicStatisticsImpl::setInt(const StatisticDescriptor* descriptor,
                                  int32_t value) {
  int32_t id = getIntId(descriptor);
  setInt(id, value);
}

void AtomicStatisticsImpl::setInt(int32_t id, int32_t value) {
  if (isOpen()) {
    _setInt(id, value);
  }
}
////////////////////////////////LONG METHODS/////////////////////////////

void AtomicStatisticsImpl::setLong(const std::string& name, int64_t value) {
  setLong(nameToDescriptor(name), value);
}

void AtomicStatisticsImpl::setLong(const StatisticDescriptor* descriptor,
                                   int64_t value) {
  setLong(getLongId(descriptor), value);
}

void AtomicStatisticsImpl::setLong(int32_t id, int64_t value) {
  if (isOpen()) {
    _setLong(id, value);
  }
}
////////////////////////////////////////DOUBLE METHODS////////////////////

void AtomicStatisticsImpl::setDouble(const std::string& name, double value) {
  setDouble(nameToDescriptor(name), value);
}

void AtomicStatisticsImpl::setDouble(const StatisticDescriptor* descriptor,
                                     double value) {
  setDouble(getDoubleId(descriptor), value);
}

void AtomicStatisticsImpl::setDouble(int32_t id, double value) {
  if (isOpen()) {
    _setDouble(id, value);
  }
}

int32_t AtomicStatisticsImpl::getInt(const std::string& name) const {
  int32_t id = getIntId(nameToDescriptor(name));
  return getInt(id);
}

int32_t AtomicStatisticsImpl::getInt(
    const StatisticDescriptor* descriptor) const {
  int32_t id = getIntId(descriptor);
  return getInt(id);
}

int32_t AtomicStatisticsImpl::getInt(int32_t id) const {
  if (isOpen()) {
    return _getInt(id);
  } else {
    return 0;
  }
}

int64_t AtomicStatisticsImpl::getLong(const std::string& name) const {
  return getLong(nameToDescriptor(name));
}

int64_t AtomicStatisticsImpl::getLong(
    const StatisticDescriptor* descriptor) const {
  return getLong(getLongId(descriptor));
}

int64_t AtomicStatisticsImpl::getLong(int32_t id) const {
  if (isOpen()) {
    return _getLong(id);
  } else {
    return 0;
  }
}

double AtomicStatisticsImpl::getDouble(const std::string& name) const {
  return getDouble(nameToDescriptor(name));
}

double AtomicStatisticsImpl::getDouble(
    const StatisticDescriptor* descriptor) const {
  return getDouble(getDoubleId(descriptor));
}

double AtomicStatisticsImpl::getDouble(int32_t id) const {
  if (isOpen()) {
    return _getDouble(id);
  } else {
    return 0;
  }
}

/*
 *Increment the value of the int32_t decriptor by delta
 */
int32_t AtomicStatisticsImpl::incInt(const std::string& name, int32_t delta) {
  int32_t id = getIntId(nameToDescriptor(name));
  return incInt(id, delta);
}

int32_t AtomicStatisticsImpl::incInt(const StatisticDescriptor* descriptor,
                                     int32_t delta) {
  int32_t id = getIntId(descriptor);
  return incInt(id, delta);
}

int32_t AtomicStatisticsImpl::incInt(int32_t id, int32_t delta) {
  if (isOpen()) {
    return _incInt(id, delta);
  } else {
    return 0;
  }
}

/*
 *Increment the value of the int64_t decriptor by delta
 */

int64_t AtomicStatisticsImpl::incLong(const std::string& name, int64_t delta) {
  return incLong(nameToDescriptor(name), delta);
}

int64_t AtomicStatisticsImpl::incLong(const StatisticDescriptor* descriptor,
                                      int64_t delta) {
  return incLong(getLongId(descriptor), delta);
}

int64_t AtomicStatisticsImpl::incLong(int32_t id, int64_t delta) {
  if (isOpen()) {
    return _incLong(id, delta);
  } else {
    return 0;
  }
}

/*
 *Increment the value of the double decriptor by delta
 */

double AtomicStatisticsImpl::incDouble(const std::string& name, double delta) {
  return incDouble(nameToDescriptor(name), delta);
}

double AtomicStatisticsImpl::incDouble(const StatisticDescriptor* descriptor,
                                       double delta) {
  return incDouble(getDoubleId(descriptor), delta);
}

double AtomicStatisticsImpl::incDouble(int32_t id, double delta) {
  if (isOpen()) {
    return _incDouble(id, delta);
  } else {
    return 0;
  }
}

int32_t AtomicStatisticsImpl::getIntId(
    const StatisticDescriptor* descriptor) const {
  const auto realDescriptor =
      dynamic_cast<const StatisticDescriptorImpl*>(descriptor);
  return realDescriptor->checkInt();
}

int32_t AtomicStatisticsImpl::getLongId(
    const StatisticDescriptor* descriptor) const {
  const auto realDescriptor =
      dynamic_cast<const StatisticDescriptorImpl*>(descriptor);
  return realDescriptor->checkLong();
}

int32_t AtomicStatisticsImpl::getDoubleId(
    const StatisticDescriptor* descriptor) const {
  const auto realDescriptor =
      dynamic_cast<const StatisticDescriptorImpl*>(descriptor);
  return realDescriptor->checkDouble();
}

}  // namespace statistics
}  // namespace geode
}  // namespace apache
