#include "storage_expression_model_store.h"

namespace Shared {

StorageExpressionModelStore::StorageExpressionModelStore() :
  m_oldestMemoizedIndex(0)
{
}

int StorageExpressionModelStore::numberOfModels() const {
  return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

Ion::Storage::Record StorageExpressionModelStore::recordAtIndex(int i) const {
  return Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
}

StorageExpressionModel * StorageExpressionModelStore::modelForRecord(Ion::Storage::Record record) const {
  uint32_t currentStorageChecksum = Ion::Storage::sharedStorage()->checksum();
  /* If the storage changed since last call to modelForRecord, we invalid all
   * memoized models. Indeed, if f(x) = A+x, and A changed, f(x) memoization
   *  which stores the reduced expression of f is outdated. */
  if (currentStorageChecksum != m_storageChecksum) {
    resetMemoizedModels();
    m_storageChecksum = currentStorageChecksum;
  } else {
    for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
      if (!memoizedModelAtIndex(i)->isNull() && *memoizedModelAtIndex(i) == record) {
        return memoizedModelAtIndex(i);
      }
    }
  }
  setMemoizedModelAtIndex(m_oldestMemoizedIndex, record);
  StorageExpressionModel * result = memoizedModelAtIndex(m_oldestMemoizedIndex);
  m_oldestMemoizedIndex = m_oldestMemoizedIndex % k_maxNumberOfMemoizedModels;
  return result;
}


void StorageExpressionModelStore::removeAll() {
  Ion::Storage::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void StorageExpressionModelStore::removeModel(Ion::Storage::Record record) {
  assert(!record.isNull());
  record.destroy();
}

void StorageExpressionModelStore::tidy() {
  for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
    memoizedModelAtIndex(i)->tidy();
  }
}

int StorageExpressionModelStore::numberOfModelsSatisfyingTest(ModelTest test) const {
  int result = 0;
  int i = 0;
  StorageExpressionModel * m = modelForRecord(recordAtIndex(i++));
  while (!m->isNull()) {
    if (test(m)) {
      result++;
    }
    m = modelForRecord(recordAtIndex(i++));
  }
  return result;
}

Ion::Storage::Record StorageExpressionModelStore::recordStatifyingTestAtIndex(int i, ModelTest test) const {
  assert(i >= 0 && i < numberOfDefinedModels());
  int index = 0;
  int currentModelIndex = 0;
  Ion::Storage::Record r = recordAtIndex(currentModelIndex++);
  StorageExpressionModel * m = modelForRecord(r);
  while (!m->isNull()) {
    assert(currentModelIndex <= numberOfModels());
    if (test(m)) {
      if (i == index) {
        return r;
      }
      index++;
    }
    r = recordAtIndex(currentModelIndex++);
    m = modelForRecord(r);
  }
  assert(false);
  return Ion::Storage::Record();
}

void StorageExpressionModelStore::resetMemoizedModels() const {
  Ion::Storage::Record emptyRecord;
  for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
    setMemoizedModelAtIndex(i, emptyRecord);
  }
}

}
