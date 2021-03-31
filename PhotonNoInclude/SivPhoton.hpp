#pragma once
#include <memory>
#include <Siv3D.hpp>


typedef unsigned char nByte;

namespace ExitGames
{
    namespace Common
    {
        class JString;

        template<class T>
        class JVector;

        class Object;

        class Hashtable;
    }

    namespace LoadBalancing {
        class Listener;

        class Player;

        class Client;
    }
}

namespace Utility
{
    /// <summary>
    /// ExitGames::Common::JStringからs3d::Stringに変換する
    /// </summary>
    /// <param name="str">変換したい文字列</param>
    /// <returns>s3d::Stringに変換した文字列</returns>
    [[nodiscard]] s3d::String ConvertJStringToString(const ExitGames::Common::JString& str);

    /// <summary>
    /// s3d::StringからExitGames::Common::JStringに変換する
    /// </summary>
    /// <param name="str">変換したい文字列</param>
    /// <returns>ExitGames::Common::JStringに変換した文字列</returns>
    [[nodiscard]] ExitGames::Common::JString ConvertStringToJString(const s3d::String& str);
}

namespace PSS
{
    class Object;
    class Hashtable;
    class Player;
}

namespace PSS
{
    class SivPhoton
    {
    private:
        class SivPhotonDetail;

        std::unique_ptr<ExitGames::LoadBalancing::Listener> m_impl;

        std::unique_ptr<ExitGames::LoadBalancing::Client> m_photonClient;

        ExitGames::LoadBalancing::Client& getClient();
    public:
        SivPhoton(const s3d::String& appID_, const s3d::String& appVersion_);

        virtual ~SivPhoton();

        void connectPhoton();

        void disconnectPhoton();

        void opCreateRoom(const s3d::String& roomName_, const s3d::HashTable<s3d::String, s3d::String>& properties_, const nByte maxPlayers_);

        void opJoinRandomRoom(const s3d::HashTable<s3d::String, s3d::String>& properties_, const nByte maxPlayers_);

        void service();

        // イベントの送信
        bool opRaiseEvent(bool reliable, const s3d::HashTable<nByte, s3d::int32>& parameters, nByte eventCode);
        bool opRaiseEvent(bool reliable, const s3d::HashTable<nByte, double>& parameters, nByte eventCode);
        bool opRaiseEvent(bool reliable, const s3d::HashTable<nByte, float>& parameters, nByte eventCode);
        bool opRaiseEvent(bool reliable, const s3d::HashTable<nByte, bool>& parameters, nByte eventCode);
        bool opRaiseEvent(bool reliable, const s3d::HashTable<nByte, s3d::String>& parameters, nByte eventCode);

        // 受信した情報を変換
        s3d::HashTable<nByte, s3d::int32> getIntDataCopy(const Object& eventContent_);
        s3d::HashTable<nByte, double> getDoubleDataCopy(const Object& eventContent_);
        s3d::HashTable<nByte, float> getFloatDataCopy(const Object& eventContent_);
        s3d::HashTable<nByte, bool> getBoolDataCopy(const Object& eventContent_);
        s3d::HashTable<nByte, s3d::String> getStringDataCopy(const Object& eventContent_);

        virtual void debugReturn(int debugLevel, const s3d::String& string);

        virtual void connectionErrorReturn(int errorCode);

        virtual void clientErrorReturn(int errorCode);

        virtual void warningReturn(int warningCode);

        virtual void serverErrorReturn(int errorCode);

        virtual void joinRoomEventAction(int playerNr, const s3d::Array<int>& playernrs, const Player& player, const bool isSelfEnter);

        virtual void leaveRoomEventAction(int playerNr, bool isInactive);

        virtual void customEventAction(int playerNr, nByte eventCode, const Object& eventContent);

        virtual void connectReturn(int errorCode, const s3d::String& errorString, const s3d::String& region, const s3d::String& cluster);

        virtual void disconnectReturn();

        virtual void leaveRoomReturn(int errorCode, const s3d::String& errorString);

        virtual void createRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const s3d::String& errorString);

        virtual void joinRandomRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const s3d::String& errorString);
    };
}