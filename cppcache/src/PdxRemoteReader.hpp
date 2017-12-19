#pragma once

#ifndef GEODE_PDXREMOTEREADER_H_
#define GEODE_PDXREMOTEREADER_H_

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

#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>
#include <sys/types.h>
#include <iosfwd>
#include <memory>

#include "PdxLocalReader.hpp"

namespace apache {
namespace geode {
namespace client {

class CacheableArrayList;
class CacheableDate;
class CacheableObjectArray;
class DataInput;
class PdxType;
class PdxTypeRegistry;
class Serializable;

class PdxRemoteReader : public PdxLocalReader {
 private:
  int32_t m_currentIndex;

 public:
  PdxRemoteReader(DataInput& dataInput, std::shared_ptr<PdxType> remoteType,
                  int32_t pdxLen,
                  std::shared_ptr<PdxTypeRegistry> pdxTypeRegistry)
      : PdxLocalReader(dataInput, remoteType, pdxLen, pdxTypeRegistry) {
    m_currentIndex = 0;
  }

  virtual ~PdxRemoteReader();

  virtual char readChar(const std::string& fieldName) override;

  virtual wchar_t readWideChar(const std::string& fieldName) override;

  /**
   * Read a boolean value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Bool value
   */
  virtual bool readBoolean(const std::string& fieldName) override;

  /**
   * Read a byte value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Byte value
   */
  virtual int8_t readByte(const std::string& fieldName) override;

  /**
   * Read a 16-bit integer value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Short value
   */
  virtual int16_t readShort(const std::string& fieldName) override;

  /**
   * Read a 32-bit integer value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Int value
   */
  virtual int32_t readInt(const std::string& fieldName) override;

  /**
   * Read a 64-bit long value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Long value
   */
  virtual int64_t readLong(const std::string& fieldName) override;

  /**
   * Read a float value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Float value
   */
  virtual float readFloat(const std::string& fieldName) override;

  /**
   * Read a double value from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Double value
   */
  virtual double readDouble(const std::string& fieldName) override;

  /**
   * Read a ASCII string from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * @param value value of the field which needs to serialize

  virtual void readASCII(const std::string& fieldName, char** value, uint16_t*
  len = nullptr);

  *
   * Read a ASCII Huge string from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * @param value value of the field which needs to serialize

  virtual void readASCIIHuge(const std::string& fieldName, char** value,
  uint32_t* len = nullptr);

  *
  * Read a UTF string from the <code>PdxReader</code>.
  * @param fieldName name of the field which needs to serialize
  * @param value value of the field which needs to serialize

  virtual void readUTF(const std::string& fieldName, char** value, uint16_t* len
  = nullptr);

  *
  * Read a string from the <code>PdxReader</code>.
  * @param fieldName name of the field which needs to serialize
  * Returns String value

  virtual void readUTFHuge(const std::string& fieldName, char** value, uint32_t*
  len = nullptr);
*/
  virtual char* readString(const std::string& fieldName) override;

  virtual wchar_t* readWideString(const std::string& fieldName) override;
  /**
   * Read a object from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * @param value value of the field which needs to serialize
   */
  virtual std::shared_ptr<Serializable> readObject(
      const std::string& fieldName) override;

  virtual char* readCharArray(const std::string& fieldName,
                              int32_t& length) override;

  virtual wchar_t* readWideCharArray(const std::string& fieldName,
                                     int32_t& length) override;

  virtual bool* readBooleanArray(const std::string& fieldName,
                                 int32_t& length) override;

  /**
   * Read a 8bit-Integer Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Byte array value
   */
  virtual int8_t* readByteArray(const std::string& fieldName,
                                int32_t& length) override;

  /**
   * Read a 16bit-Integer Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Short
   */
  virtual int16_t* readShortArray(const std::string& fieldName,
                                  int32_t& length) override;

  /**
   * Read a 32bit-Integer Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Int value
   */
  virtual int32_t* readIntArray(const std::string& fieldName,
                                int32_t& length) override;

  /**
   * Read a Long integer Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Long array
   */
  virtual int64_t* readLongArray(const std::string& fieldName,
                                 int32_t& length) override;

  /**
   * Read a Float Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Float array
   */
  virtual float* readFloatArray(const std::string& fieldName,
                                int32_t& length) override;

  /**
   * Read a Double Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns Double array
   */
  virtual double* readDoubleArray(const std::string& fieldName,
                                  int32_t& length) override;

  /**
   * Read a String Array from the <code>PdxReader</code>.
   * @param fieldName name of the field which needs to serialize
   * Returns String array
   */
  virtual char** readStringArray(const std::string& fieldName,
                                 int32_t& length) override;

  virtual wchar_t** readWideStringArray(const std::string& fieldName,
                                        int32_t& length) override;

  virtual std::shared_ptr<CacheableObjectArray> readObjectArray(
      const std::string& fieldName) override;

  virtual int8_t** readArrayOfByteArrays(const std::string& fieldName,
                                         int32_t& arrayLength,
                                         int32_t** elementLength) override;

  virtual std::shared_ptr<CacheableDate> readDate(
      const std::string& fieldName) override;

  /*virtual void readBytes(const std::string& fieldName, uint8_t*& bytes,
    int32_t& len);

  virtual void readBytes(const std::string& fieldName, int8_t*& bytes,
    int32_t& len );

  virtual void readASCIIChar(const std::string& fieldName, wchar_t& value);*/

  virtual void readCollection(
      const std::string& fieldName,
      std::shared_ptr<CacheableArrayList>& collection) override;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PDXREMOTEREADER_H_
