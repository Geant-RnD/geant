//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h
#include <atomic>
#include <memory>
#include <exception>
#include "ModuleWrapper.h"
#include "PrefetchAndWorkWrapper.h"
#include "WaitingTaskList.h"
#include "Producer.h"

namespace demo {
  
  class Event;
  
  class ProducerWrapper : private ModuleWrapper,PrefetchAndWorkWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    ///Pass task to be called when data has been produced
    void doProduceAsync(WaitingTask* iCallTaskWhenDone);

    void reset();

    bool wasRun() const {return m_wasRun;}
    
    unsigned int id() const { return m_producer->id(); }
  private:
    void doWork(std::exception_ptr) override;
    Producer* producer() const;

    ProducerWrapper& operator=(const ProducerWrapper&);
    
    std::shared_ptr<Producer> m_producer;
    WaitingTaskList m_waitingList;
    bool m_wasRun;

  };
}

#endif
