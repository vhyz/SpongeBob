#pragma once

#include <vector>

struct epoll_event;

namespace sponge {

class Event;

class Poller {
   public:
    Poller();
    ~Poller();

    // 等待事件
    void poll(int timeout);

    // 处理活跃事件
    void processActiveEvents();

    // 添加事件
    void addEvent(Event* event);

    // 更新事件
    void updateEvent(Event* event);

    // 删除事件
    void removeEvent(Event* event); 

   private:
    int epollfd_;

    std::vector<epoll_event> eventList_;
};

}  // namespace sponge