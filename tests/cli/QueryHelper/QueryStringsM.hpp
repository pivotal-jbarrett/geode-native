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

#pragma once

#include "QueryStrings.hpp"
#include "native_conditional_unique_ptr.hpp"


using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {
      namespace Tests
      {

        /// <summary>
        /// Specifies the category of a query
        /// </summary>
        PUBLIC enum class QueryCategory
        {
          SingleRegion = 0,
          MultiRegion,
          Operators,
          Constants,
          Functions,
          CollectionOps,
          Keywords,
          RegionInterface,
          NestedQueries,
          ImportAndSelect,
          Canonicalization,
          Unsupported,
          QueryAndIndexing,
          Misc,
          RegionQuery,
          QcEnd // to mark the end of enum
        };


        /// <summary>
        /// Encapsulates a query string.
        /// </summary>
        PUBLIC ref class QueryStrings sealed
        {
        public:

          inline QueryStrings( QueryCategory pcategory, gc_ptr(String) pquery,
            Boolean pisLargeResultset )
          {
            Init( pcategory, pquery, pisLargeResultset );
          }

          QueryStrings( QueryCategory pcategory, gc_ptr(String) pquery )
          {
            Init( pcategory, pquery, false );
          }

          static property Int32 RSsize
          {
            Int32 get( );
          }

          static property Int32 RSPsize
          {
            Int32 get( );
          }

          static property Int32 SSsize
          {
            Int32 get( );
          }

          static property Int32 SSPsize
          {
            Int32 get( );
          }

          static property Int32 RQsize
          {
            Int32 get( );
          }

          static property Int32 CQRSsize
          {
            Int32 get( );
          }

          property QueryCategory Category
          {
            QueryCategory get( );
          }

          property gc_ptr(String) Query
          {
            gc_ptr(String) get( );
          }

          property bool IsLargeResultset
          {
            bool get( );
          }

        private:

          void Init( QueryCategory pcategory, gc_ptr(String) pquery,
            Boolean pisLargeResultset );

          gc_ptr(native_conditional_unique_ptr<testData::QueryStrings>) m_nativeptr;

        CLI(internal:)

          /// <summary>
          /// Internal constructor to wrap a native object pointer
          /// </summary>
          /// <param name="nativeptr">The native object pointer</param>
          inline QueryStrings(testData::QueryStrings* nativeptr)
          {
            m_nativeptr = gcnew native_conditional_unique_ptr<testData::QueryStrings>(nativeptr);
          }
        };

        /// <summary>
        /// Contains static query arrays and their expected results.
        /// </summary>
        PUBLIC ref class QueryStatics sealed
        {
        public:

          static gc_ptr(array<QueryStrings^>) ResultSetQueries;

          static gc_ptr(array<QueryStrings^>) ResultSetParamQueries;

          static array<gc_ptr(array<String^>)>^ QueryParamSet;

          static array<gc_ptr(array<String^>)>^ QueryParamSetSS;

          static gc_ptr(array<Int32>) NoOfQueryParam;

          static gc_ptr(array<Int32>) NoOfQueryParamSS;

          static gc_ptr(array<Int32>) ResultSetRowCounts;

          static gc_ptr(array<Int32>) ResultSetPQRowCounts;

          static gc_ptr(array<Int32>) ConstantExpectedRowsRS;

          static gc_ptr(array<Int32>) ConstantExpectedRowsPQRS;

          static gc_ptr(array<QueryStrings^>) StructSetQueries;

          static gc_ptr(array<QueryStrings^>) StructSetParamQueries;

          static gc_ptr(array<Int32>) StructSetRowCounts;

          static gc_ptr(array<Int32>) StructSetPQRowCounts;

          static gc_ptr(array<Int32>) StructSetFieldCounts;

          static gc_ptr(array<Int32>) StructSetPQFieldCounts;

          static gc_ptr(array<Int32>) ConstantExpectedRowsSS;

          static gc_ptr(array<Int32>) ConstantExpectedRowsPQSS;

          static gc_ptr(array<QueryStrings^>) RegionQueries;

          static gc_ptr(array<Int32>) RegionQueryRowCounts;

          static gc_ptr(array<QueryStrings^>) CqResultSetQueries;

          static gc_ptr(array<Int32>) CqResultSetRowCounts;

          static gc_ptr(array<Int32>) ConstantExpectedRowsCQRS;

        private:

          static QueryStatics( );
        };

      }
    }
  }
}
