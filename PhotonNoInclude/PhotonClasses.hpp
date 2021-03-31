#pragma once
#include "SivPhoton.hpp"

namespace PSS
{
    class Object
    {
    private:
        std::unique_ptr<ExitGames::Common::Object> m_object;

    public:
        Object();
        Object(const ExitGames::Common::Object& object_);
        Object(const Object& object_);

        ~Object();

        Object& operator=(const Object& toCopy);

        bool operator==(const Object& toCompare) const;
        bool operator!=(const Object& toCompare) const;

        nByte getType(void) const;
        nByte getCustomType(void) const;
        const short* getSizes(void) const;
        unsigned int getDimensions(void) const;

        ExitGames::Common::Object& getObject() const;
    };

    class Hashtable
    {
    private:
        std::unique_ptr<ExitGames::Common::Hashtable> m_hashtable;

    public:
        Hashtable(const ExitGames::Common::Hashtable& hashtable_);

        ~Hashtable();

        Hashtable& operator=(const Hashtable& toCopy);

        bool operator==(const Hashtable& toCompare) const;
        bool operator!=(const Hashtable& toCompare) const;
        const Object& operator[](unsigned int index) const;

        void put(const ExitGames::Common::Hashtable& src);

        template<typename FKeyType, typename FValueType>
        void put(const FKeyType& key, const FValueType& val);

        template<typename FKeyType>
        void put(const FKeyType& key);

        template<typename FKeyType, typename FValueType>
        void put(const FKeyType& key, const FValueType pVal, const short* sizes);

        template<typename FKeyType>
        const Object* getValue(const FKeyType& key) const;

        unsigned int getSize(void) const;

        const s3d::Array<Object> getKeys(void) const;

        template<typename FKeyType>
        void remove(const FKeyType& key);

        template<typename FKeyType>
        bool contains(const FKeyType& key) const;

        void removeAllElements(void);
    };

    class Player
    {
    private:
        std::unique_ptr<ExitGames::LoadBalancing::Player> m_player;

    public:
        Player(const ExitGames::LoadBalancing::Player& player_);

        ~Player();

        Player& operator=(const Player& toCopy);

        int getNumber(void) const;
        const s3d::String getName() const;
        const s3d::String getUserID() const;
        Hashtable getCustomProperties() const;
        bool getIsInactive(void) const;
        bool getIsMasterClient(void) const;

        bool operator==(const Player& player) const;

        s3d::String toString(bool withTypes, bool withCustomProperties) const;
    };
}
