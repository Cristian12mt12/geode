#pragma once

#include <Interface.hpp>
#include <string>
#include <typeinfo>
#include <memory>
#include <exception>
#include <unordered_map>
#include <gen/Header.hpp>
#include <any>

#ifndef GEODE_DLL
#define GEODE_DLL
#endif

namespace geode {
    class NotificationCenter;
    class Mod;

    class GEODE_DLL Notification {
    protected:
        std::string m_selector;
        std::any m_object;
        Mod* m_sender;
    public:
        template <typename T>
        T const& object() const {
        	return std::any_cast<T const&>(m_object);
        }

        inline std::string const& selector() const { return m_selector; }
        inline Mod* sender() const { return m_sender; }

        template <typename T>
        Notification(std::string sel, T&& obj, Mod* sender) : 
            m_selector(sel),
            m_object(std::forward<T>(obj)),
            m_sender(sender) {}

        Notification(std::string sel, Mod* sender) : Notification(sel, nullptr, sender) {}
        Notification(std::string sel) : Notification(sel, Interface::get()->mod()) {}

        Notification(Notification&& a) : m_selector(a.m_selector), m_sender(a.m_sender), m_object(std::move(a.m_object)) {}

        friend class NotificationCenter;
    };

    struct Observer {
        std::string m_selector;
        Mod* m_mod;
        std::function<void(Notification const&)> m_callback;
    };

    class GEODE_DLL NotificationCenter {
    protected:
        std::map<Mod*, std::map<std::string, std::vector<Observer*>>> m_observers;
        static NotificationCenter* shared;
    public:
        using callback_t = std::function<void(Notification const&)>;

        NotificationCenter();
        static NotificationCenter* get();

        void send(Notification n, Mod* m);
        void broadcast(Notification n);

        Observer* registerObserver(Mod* m, std::string selector, callback_t cb);
        inline Observer* registerObserver(std::string selector, callback_t cb) {
            return registerObserver(Interface::get()->mod(), selector, cb);
        }
        void unregisterObserver(Observer* ob);
        std::vector<Observer*> getObservers(std::string selector, Mod* m);
    };
}
