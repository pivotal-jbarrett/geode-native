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

#pragma warning(disable : 4091)
#include <msclr/lock.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
// using namespace System::WeakReference;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
PUBLIC ref class EntryNode {
  gc_ptr(System::WeakReference) m_weakValue;
  gc_ptr(EntryNode) m_nextEntryNode;

 public:
  EntryNode(gc_ptr(Object) val, gc_ptr(EntryNode) nextNode) {
    m_weakValue = gcnew WeakReference(val);
    m_nextEntryNode = nextNode;
  }

  bool isValEquals(gc_ptr(Object) other) {
    if (other == nullptr) return false;
    gc_ptr(Object) val = m_weakValue->Target;

    if (val != nullptr) {
      if (val->GetHashCode() == other->GetHashCode() && val->Equals(other)) return true;
    }

    return false;
  }

  property gc_ptr(Object) Value {
    gc_ptr(Object) get() { return m_weakValue->Target; }
    void set(gc_ptr(Object) val) { m_weakValue = gcnew WeakReference(val); }
  }

  property gc_ptr(EntryNode) NextNode {
    gc_ptr(EntryNode) get() { return m_nextEntryNode; }
  }
};

PUBLIC ref class MapEntry {
  gc_ptr(EntryNode) m_entryNode;

 public:
  MapEntry(gc_ptr(Object) newVal) { m_entryNode = gcnew EntryNode(newVal, nullptr); }

  void AddEntry(gc_ptr(Object) newVal) {
    // lock at entry level
    msclr::lock lockInstance(this);
    // 1. see if hashCode and equals matches if yes then replace.
    // 2. store node if val is null(GC collected ) replace that node(ie reuse)
    // 3. create new node

    gc_ptr(EntryNode) startNode = m_entryNode;
    gc_ptr(EntryNode) nullNode = nullptr;  // to replace

    while (startNode != nullptr) {
      if (startNode->isValEquals(newVal)) {
        // replace value from new value;; will we ever come acrosss here ???
        startNode->Value = newVal;
        return;
      } else if (nullNode == nullptr && startNode->Value == nullptr) {
        // reuse the node once it gced
        nullNode = startNode;
      }
      startNode = startNode->NextNode;
    }

    if (nullNode != nullptr) {
      nullNode->Value = newVal;
    } else {
      // create new node
      gc_ptr(EntryNode) newNode = gcnew EntryNode(newVal, m_entryNode);
      m_entryNode = newNode;  // append new node as put can happen immediately for this
    }
  }

  gc_ptr(Object) Get(gc_ptr(Object) val) {
    gc_ptr(EntryNode) startNode = m_entryNode;

    while (startNode != nullptr) {
      if (startNode->isValEquals(val)) {
        return startNode->Value;
      }
      startNode = startNode->NextNode;
    }

    return nullptr;
  }
};

PUBLIC ref class WeakHashMap {
  // not thread safe
  gc_ptr(Dictionary<int, gc_ptr(MapEntry)>) m_dictionary;
  gc_ptr(ReaderWriterLock) m_readerWriterLock;

 public:
  WeakHashMap() {
    m_dictionary = gcnew Dictionary<int, gc_ptr(MapEntry)>(1000);
    m_readerWriterLock = gcnew ReaderWriterLock();
  }

  property int Count {
    int get() { return m_dictionary->Count; }
  }

  void Put(gc_ptr(Object) val) {
    int hash = val->GetHashCode();

    gc_ptr(MapEntry) ret = GetEntry(val);

    if (ret != nullptr) {
      // this will take lock at entry level
      // so once entry is created it will remain there and we don't need to take writer lock
      ret->AddEntry(val);
    } else {
      try {
        m_readerWriterLock->AcquireWriterLock(-1);

        m_dictionary->TryGetValue(hash, ret);

        if (ret != nullptr) {
          ret->AddEntry(val);
        } else {
          ret = gcnew MapEntry(val);
          m_dictionary[hash] = ret;
        }
      } finally {
        m_readerWriterLock->ReleaseWriterLock();
      }
    }
  }

  gc_ptr(MapEntry) GetEntry(gc_ptr(Object) val) {
    int hash = val->GetHashCode();

    gc_ptr(MapEntry) ret = nullptr;

    try {
      m_readerWriterLock->AcquireReaderLock(-1);  // infinite timeout
      m_dictionary->TryGetValue(hash, ret);
    } finally {
      m_readerWriterLock->ReleaseReaderLock();
    }
    return ret;
  }

  gc_ptr(Object) Get(gc_ptr(Object) val) {
    if (m_dictionary->Count > 0) {
      int hash = val->GetHashCode();

      gc_ptr(MapEntry) ret = nullptr;

      try {
        m_readerWriterLock->AcquireReaderLock(-1);  // infinite timeout
        m_dictionary->TryGetValue(hash, ret);

        if (ret != nullptr) {
          return ret->Get(val);
        }
      } finally {
        m_readerWriterLock->ReleaseReaderLock();
      }
    }
    return nullptr;
  }

  void Clear() { m_dictionary->Clear(); }
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
