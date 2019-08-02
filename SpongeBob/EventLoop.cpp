#include "EventLoop.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>

int createEventFd() {
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0) {
        std::cout << "eventfd error" << std::endl;
        exit(1);
    }
    return fd;
}

EventLoop::EventLoop()
    : poller_(),
      avtiveChannelList_(),
      quit_(false),
      threadId_(std::this_thread::get_id()),
      mutex_(),
      taskList_(),
      wakeUpFd_(-1) {
    wakeUpFd_ = createEventFd();
    wakeChannel_.setFd(wakeUpFd_);
    wakeChannel_.setEvents(EPOLLIN);
    wakeChannel_.setReadCallBack(std::bind(&EventLoop::readWakeUpHandle, this));
    addChannel(&wakeChannel_);
}

void EventLoop::loop() {
    quit_ = false;
    std::cout << "loop start" << std::endl;
    while (!quit_) {
        poller_.poll(avtiveChannelList_);

        for (Channel* channel : avtiveChannelList_) {
            channel->handleEvent();
        }

        avtiveChannelList_.clear();

        // 每一次唤醒都要执行一下任务
        // 确保事件的变化
        runTasks();
    }
}

void EventLoop::readWakeUpHandle() {
    //std::cout << "wakeUpRead" << std::endl;
    uint64_t one = 1;
    read(wakeUpFd_, &one, sizeof(one));
}

void EventLoop::wakeUp() {
    uint64_t one = 1;
    write(wakeUpFd_, (char*)&one, sizeof(one));
}

void EventLoop::addTask(Functor functor) {
    {
        std::lock_guard guard(mutex_);
        taskList_.push_back(std::move(functor));
    }
    wakeUp();
}

void EventLoop::runTasks() {
    std::vector<Functor> taskList;
    {
        std::lock_guard guard(mutex_);
        taskList.swap(taskList_);
    }
    for (size_t i = 0; i < taskList.size(); ++i) {
        taskList[i]();
    }
}

bool EventLoop::runInLoop() { return threadId_ == std::this_thread::get_id(); }