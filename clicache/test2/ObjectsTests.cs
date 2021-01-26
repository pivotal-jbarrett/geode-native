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

using System;
using Xunit;

namespace Apache.Geode.Client
{

  public class ObjectsTests
  {

    [Fact]
    public void GetHashCodeNullObjectIsZero()
    {
      Assert.Equal(0, Objects.GetHashCode((Object)null));
    }

    [Fact]
    public void GetHashCodeOfEmptyArrayIs1()
    {
      Assert.Equal(1, Objects.GetHashCode(new Object[0]));
    }

    [Fact]
    public void HashCodeEmptyArgsIs1()
    {
      Assert.Equal(1, Objects.Hash());
    }

    [Fact]
    public void HashCodeOfString()
    {
      Assert.Equal(0, Objects.GetHashCode(""));
      Assert.Equal(48, Objects.GetHashCode("0"));
      Assert.Equal(57, Objects.GetHashCode("9"));
      Assert.Equal(97, Objects.GetHashCode("a"));
      Assert.Equal(122, Objects.GetHashCode("z"));
      Assert.Equal(65, Objects.GetHashCode("A"));
      Assert.Equal(90, Objects.GetHashCode("Z"));

      Assert.Equal(1077910243, Objects.GetHashCode("supercalifragilisticexpialidocious"));

      Assert.Equal(1544552287, Objects.GetHashCode("You had me at meat tornad\u00F6!\U000F0000"));

      Assert.Equal(701776767, Objects.GetHashCode("You had me at\0meat tornad\u00F6!\U000F0000"));

      Assert.Equal(
          512895612,
          Objects.GetHashCode("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
               + "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
               + "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
               + "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
               + "reprehenderit in voluptate velit esse cillum dolore eu fugiat "
               + "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
               + "sunt in culpa qui officia deserunt mollit anim id est laborum."));

      Assert.Equal(
          -1425027716,
          Objects.GetHashCode("\u16bb\u16d6\u0020\u16b3\u16b9\u16ab\u16a6\u0020\u16a6\u16ab"
               + "\u16cf\u0020\u16bb\u16d6\u0020\u16d2\u16a2\u16de\u16d6\u0020"
               + "\u16a9\u16be\u0020\u16a6\u16ab\u16d7\u0020\u16da\u16aa\u16be"
               + "\u16de\u16d6\u0020\u16be\u16a9\u16b1\u16a6\u16b9\u16d6\u16aa"
               + "\u16b1\u16de\u16a2\u16d7\u0020\u16b9\u16c1\u16a6\u0020\u16a6"
               + "\u16aa\u0020\u16b9\u16d6\u16e5\u16ab"));
    }

    [Fact]
    public void HashCodeOf1String()
    {
      Assert.Equal(121, Objects.Hash("Z"));
    }

    [Fact]
    public void GetHashCodeOfChar()
    {
      Assert.Equal(48, Objects.GetHashCode('0'));
      Assert.Equal(57, Objects.GetHashCode('9'));
      Assert.Equal(97, Objects.GetHashCode('a'));
      Assert.Equal(122, Objects.GetHashCode('z'));
      Assert.Equal(65, Objects.GetHashCode('A'));
      Assert.Equal(90, Objects.GetHashCode('Z'));
    }

    [Fact]
    public void HashCodeOf1Char()
    {
      Assert.Equal(121, Objects.Hash('Z'));
    }

    [Fact]
    public void GetHashCodeOfBoolean()
    {
      Assert.Equal(1237, Objects.GetHashCode(false));
      Assert.Equal(1231, Objects.GetHashCode(true));
    }

    [Fact]
    public void HashCodeOf1Boolean()
    {
      Assert.Equal(1262, Objects.Hash(true));
    }

    [Fact]
    public void GetHashCodeOfByte()
    {
      Assert.Equal(0, Objects.GetHashCode((SByte)0));
      Assert.Equal(1, Objects.GetHashCode((SByte)1));
      Assert.Equal(-1, Objects.GetHashCode((SByte)(-1)));
      Assert.Equal(SByte.MaxValue, Objects.GetHashCode(SByte.MaxValue));
      Assert.Equal(SByte.MinValue, Objects.GetHashCode(SByte.MinValue));
    }

    [Fact]
    public void HashCodeOf1Byte()
    {
      Assert.Equal(158, Objects.Hash(SByte.MaxValue));
    }

    [Fact]
    public void GetHashCodeOfInt16()
    {
      Assert.Equal(0, Objects.GetHashCode((Int16)0));
      Assert.Equal(1, Objects.GetHashCode((Int16)1));
      Assert.Equal(-1, Objects.GetHashCode((Int16)(-1)));
      Assert.Equal(Int16.MaxValue, Objects.GetHashCode(Int16.MaxValue));
      Assert.Equal(Int16.MinValue, Objects.GetHashCode(Int16.MinValue));
    }

    [Fact]
    public void HashCodeOf1Int16()
    {
      Assert.Equal(32798, Objects.Hash(Int16.MaxValue));
    }

    [Fact]
    public void GetHashCodeOfInt32()
    {
      Assert.Equal(0, Objects.GetHashCode(0));
      Assert.Equal(1, Objects.GetHashCode(1));
      Assert.Equal(-1, Objects.GetHashCode(-1));
      Assert.Equal(Int32.MaxValue, Objects.GetHashCode(Int32.MaxValue));
      Assert.Equal(Int32.MinValue, Objects.GetHashCode(Int32.MinValue));
    }

    [Fact]
    public void HashCodeOf1Int32()
    {
      Assert.Equal(-2147483618, Objects.Hash(Int32.MaxValue));
    }

    [Fact]
    public void GetHashCodeOfInt64()
    {
      Assert.Equal(0, Objects.GetHashCode(0L));
      Assert.Equal(1, Objects.GetHashCode(1L));
      Assert.Equal(0, Objects.GetHashCode(-1L));
      Assert.Equal(-2147483648, Objects.GetHashCode(((Int64)Int32.MaxValue) + 1));
      Assert.Equal(-2147483648, Objects.GetHashCode(((Int64)Int32.MinValue) - 1));
      Assert.Equal(-2147483648, Objects.GetHashCode(Int64.MaxValue));
      Assert.Equal(-2147483648, Objects.GetHashCode(Int64.MinValue));
      Assert.Equal(-1073741824, Objects.GetHashCode(Int64.MaxValue >> 1));
      Assert.Equal(-1073741824, Objects.GetHashCode(Int64.MinValue >> 1));
      Assert.Equal(-536870912, Objects.GetHashCode(Int64.MaxValue >> 2));
      Assert.Equal(-536870912, Objects.GetHashCode(Int64.MinValue >> 2));
      Assert.Equal(0, Objects.GetHashCode(-9223372034707292160L));
    }

    [Fact]
    public void HashCodeOf1Int64()
    {
      Assert.Equal(-2147483617, Objects.Hash(Int64.MaxValue));
    }

    [Fact]
    public void GetHashCodeOfFloat()
    {
      Assert.Equal(0, Objects.GetHashCode(0.0f));
      Assert.Equal(-2147483648, Objects.GetHashCode(-0.0f));
      Assert.Equal(1065353216, Objects.GetHashCode(1.0f));
      Assert.Equal(-1082130432, Objects.GetHashCode(-1.0f));
      Assert.Equal(2139095039, Objects.GetHashCode(Single.MaxValue));
      Assert.Equal(1, Objects.GetHashCode(Single.Epsilon));
      Assert.Equal(-8388609, Objects.GetHashCode(Single.MinValue));
      Assert.Equal(2139095040, Objects.GetHashCode(Single.PositiveInfinity));
      Assert.Equal(-8388608, Objects.GetHashCode(Single.NegativeInfinity));
      Assert.Equal(8388608, Objects.GetHashCode(1.17549435E-38f));
      Assert.Equal(2143289344, Objects.GetHashCode(Single.NaN));
    }

    [Fact]
    public void HashCodeOf1Float()
    {
      Assert.Equal(2139095070, Objects.Hash(Single.MaxValue));
    }

    [Fact]
    public void GetHashCodeOfDouble()
    {
      Assert.Equal(0, Objects.GetHashCode(0.0d));
      Assert.Equal(-2147483648, Objects.GetHashCode(-0.0d));
      Assert.Equal(1072693248, Objects.GetHashCode(1.0d));
      Assert.Equal(-1074790400, Objects.GetHashCode(-1.0d));
      Assert.Equal(-2146435072, Objects.GetHashCode(Double.MaxValue));
      Assert.Equal(1, Objects.GetHashCode(Double.Epsilon));
      Assert.Equal(1048576, Objects.GetHashCode(Double.MinValue));
      Assert.Equal(2146435072, Objects.GetHashCode(Double.PositiveInfinity));
      Assert.Equal(-1048576, Objects.GetHashCode(Double.NegativeInfinity));
      Assert.Equal(1048576, Objects.GetHashCode(2.2250738585072014E-308d));
      Assert.Equal(2146959360, Objects.GetHashCode(Double.NaN));
    }

    [Fact]
    public void HashCodeOf1Double()
    {
      Assert.Equal(-2146435041, Objects.Hash(Double.MaxValue));
    }


    [Fact]
    public void GetHashCodeOfDateTime()
    {
      Assert.Equal(0, Objects.GetHashCode(new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
      Assert.Equal(1583802735, Objects.GetHashCode(new DateTime(2020, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
      Assert.Equal(-927080926, Objects.GetHashCode(new DateTime(3020, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
      Assert.Equal(1670202000, Objects.GetHashCode(new DateTime(1920, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
      Assert.Equal(542840753, Objects.GetHashCode(new DateTime(1820, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
    }

    [Fact]
    public void HashCodeOf1DateTime()
    {
      Assert.Equal(31, Objects.Hash(new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
    }

  }

};
