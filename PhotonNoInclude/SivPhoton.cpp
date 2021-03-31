#define NO_S3D_USING
#include "SivPhoton.hpp"
#include "PhotonClasses.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# if SIV3D_BUILD_TYPE(DEBUG)
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_debug_windows_mt_x64")
# else
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_release_windows_mt_x64")
# endif
# endif

#include <LoadBalancing-cpp/inc/Client.h>

namespace Utility
{
    /// <summary>
    /// ExitGames::Common::JStringからs3d::Stringに変換する
    /// </summary>
    /// <param name="str">変換したい文字列</param>
    /// <returns>s3d::Stringに変換した文字列</returns>
    s3d::String ConvertJStringToString(const ExitGames::Common::JString& str)
    {
        return s3d::Unicode::FromWString(std::wstring(str));
    }


    /// <summary>
    /// s3d::StringからExitGames::Common::JStringに変換する
    /// </summary>
    /// <param name="str">変換したい文字列</param>
    /// <returns>ExitGames::Common::JStringに変換した文字列</returns>
    ExitGames::Common::JString ConvertStringToJString(const s3d::String& str)
    {
        return ExitGames::Common::JString(str.toWstr().c_str());
    }
}

namespace PSS
{
    class SivPhoton::SivPhotonDetail : public ExitGames::LoadBalancing::Listener
    {
    private:
        SivPhoton& m_context;
    public:

        SivPhotonDetail(SivPhoton& context_)
            : m_context(context_)
        {}
        
        virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override
        {
            m_context.debugReturn(debugLevel, Utility::ConvertJStringToString(string));
        }

        virtual void connectionErrorReturn(int errorCode) override
        {
            m_context.connectionErrorReturn(errorCode);
        }

        virtual void clientErrorReturn(int errorCode) override
        {
            m_context.clientErrorReturn(errorCode);
        }

        virtual void warningReturn(int warningCode) override
        {
            m_context.warningReturn(warningCode);
        }

        virtual void serverErrorReturn(int errorCode) override
        {
            m_context.serverErrorReturn(errorCode);
        }

        virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override
        {
            s3d::Array<int> v(playernrs.getSize());
            for (unsigned int i{}; i < playernrs.getSize(); ++i)
            {
                v[i] = playernrs.getElementAt(i);
            }

            const bool isSelfEnter{ m_context.getClient().getLocalPlayer().getNumber() == player.getNumber() };
            m_context.joinRoomEventAction(playerNr, v, { player }, isSelfEnter);
        }

        virtual void leaveRoomEventAction(int playerNr, bool isInactive) override
        {
            m_context.leaveRoomEventAction(playerNr, isInactive);
        }

        virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override
        {
            m_context.customEventAction(playerNr, eventCode, { eventContent });
        }

        virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override
        {
            m_context.connectReturn(errorCode, Utility::ConvertJStringToString(errorString), Utility::ConvertJStringToString(region), Utility::ConvertJStringToString(cluster));
        }

        virtual void disconnectReturn() override
        {
            m_context.disconnectReturn();
        }

        virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override
        {
            m_context.leaveRoomReturn(errorCode, Utility::ConvertJStringToString(errorString));
        }

        virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& roomProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override
        {
            m_context.createRoomReturn(localPlayerNr, { roomProperties }, { playerProperties }, errorCode, Utility::ConvertJStringToString(errorString));
        }

        virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& roomProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override
        {
            m_context.joinRandomRoomReturn(localPlayerNr, { roomProperties }, { playerProperties }, errorCode, Utility::ConvertJStringToString(errorString));
        }
    };

    SivPhoton::SivPhoton(const s3d::String& appID_, const s3d::String& appVersion_)
        : m_impl{ std::make_unique<SivPhotonDetail>(*this) }
        , m_photonClient{ std::make_unique<ExitGames::LoadBalancing::Client>(*m_impl, Utility::ConvertStringToJString(appID_), Utility::ConvertStringToJString(appVersion_)) }
    {}

    SivPhoton::~SivPhoton()
    {
        disconnectPhoton();
    }

    ExitGames::LoadBalancing::Client& SivPhoton::getClient() {
        return *m_photonClient;
    }

    void SivPhoton::connectPhoton()
    {
        getClient().setAutoJoinLobby(true);

        if (!getClient().connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS()))) {
            return;
        }

        getClient().fetchServerTimestamp();
    }

    void SivPhoton::disconnectPhoton()
    {
        getClient().disconnect();
    }

    void SivPhoton::opCreateRoom(const s3d::String& roomName_, const s3d::HashTable<s3d::String, s3d::String>& properties_, const nByte maxPlayers_)
    {
        ExitGames::Common::Hashtable properties;
        for (const auto& [key, value] : properties_)
        {
            properties.put(Utility::ConvertStringToJString(key), Utility::ConvertStringToJString(value));
        }

        getClient().opCreateRoom(Utility::ConvertStringToJString(roomName_), ExitGames::LoadBalancing::RoomOptions().setMaxPlayers(maxPlayers_).setCustomRoomProperties(properties));
    }

    void SivPhoton::opJoinRandomRoom(const s3d::HashTable<s3d::String, s3d::String>& properties_, const nByte maxPlayers_)
    {
        ExitGames::Common::Hashtable properties;
        for (const auto& [key, value] : properties_)
        {
            properties.put(Utility::ConvertStringToJString(key), Utility::ConvertStringToJString(value));
        }

        getClient().opJoinRandomRoom(properties, maxPlayers_);
    }

    void SivPhoton::service()
    {
        getClient().service();
    }

    bool SivPhoton::opRaiseEvent(bool reliable_, const s3d::HashTable<nByte, s3d::int32>& parameters_, nByte eventCode_)
    {
        ExitGames::Common::Dictionary<nByte, s3d::int32> dic;
        for (const auto& [key, value] : parameters_)
        {
            dic.put(key, value);
        }
        return getClient().opRaiseEvent(reliable_, dic, eventCode_);
    }
    bool SivPhoton::opRaiseEvent(bool reliable_, const s3d::HashTable<nByte, double>& parameters_, nByte eventCode_)
    {
        ExitGames::Common::Dictionary<nByte, double> dic;
        for (const auto& [key, value] : parameters_)
        {
            dic.put(key, value);
        }
        return getClient().opRaiseEvent(reliable_, dic, eventCode_);
    }
    bool SivPhoton::opRaiseEvent(bool reliable_, const s3d::HashTable<nByte, float>& parameters_, nByte eventCode_)
    {
        ExitGames::Common::Dictionary<nByte, float> dic;
        for (const auto& [key, value] : parameters_)
        {
            dic.put(key, value);
        }
        return getClient().opRaiseEvent(reliable_, dic, eventCode_);
    }
    bool SivPhoton::opRaiseEvent(bool reliable_, const s3d::HashTable<nByte, bool>& parameters_, nByte eventCode_)
    {
        ExitGames::Common::Dictionary<nByte, bool> dic;
        for (const auto& [key, value] : parameters_)
        {
            dic.put(key, value);
        }
        return getClient().opRaiseEvent(reliable_, dic, eventCode_);
    }
    bool SivPhoton::opRaiseEvent(bool reliable_, const s3d::HashTable<nByte, s3d::String>& parameters_, nByte eventCode_)
    {
        ExitGames::Common::Dictionary<nByte, const char*> dic;
        for (const auto& [key, value] : parameters_)
        {
            dic.put(key, s3d::Unicode::Narrow(value).c_str());
        }
        return getClient().opRaiseEvent(reliable_, dic, eventCode_);
    }

    s3d::HashTable<nByte, s3d::int32> SivPhoton::getIntDataCopy(const Object& eventContent_)
    {
        s3d::HashTable<nByte, s3d::int32> ret;
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, s3d::int32>>(eventContent_.getObject()).getDataCopy();
        s3d::Array<nByte> retKeys;
        {
            auto keys = dic.getKeys();
            for (unsigned int i{}; i < keys.getSize(); ++i)
            {
                retKeys.emplace_back(keys[i]);
            }
        }

        for (const nByte& key : retKeys)
        {
            ret.emplace(key, *dic.getValue(key));
        }

        return ret;
    }

    s3d::HashTable<nByte, double> SivPhoton::getDoubleDataCopy(const Object& eventContent_)
    {
        s3d::HashTable<nByte, double> ret;
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, double>>(eventContent_.getObject()).getDataCopy();
        s3d::Array<nByte> retKeys;
        {
            auto keys = dic.getKeys();
            for (unsigned int i{}; i < keys.getSize(); ++i)
            {
                retKeys.emplace_back(keys[i]);
            }
        }

        for (const nByte& key : retKeys)
        {
            ret.emplace(key, *dic.getValue(key));
        }

        return ret;
    }

    s3d::HashTable<nByte, float> SivPhoton::getFloatDataCopy(const Object& eventContent_)
    {
        s3d::HashTable<nByte, float> ret;
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, float>>(eventContent_.getObject()).getDataCopy();
        s3d::Array<nByte> retKeys;
        {
            auto keys = dic.getKeys();
            for (unsigned int i{}; i < keys.getSize(); ++i)
            {
                retKeys.emplace_back(keys[i]);
            }
        }

        for (const nByte& key : retKeys)
        {
            ret.emplace(key, *dic.getValue(key));
        }

        return ret;
    }

    s3d::HashTable<nByte, bool> SivPhoton::getBoolDataCopy(const Object& eventContent_)
    {
        s3d::HashTable<nByte, bool> ret;
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, bool>>(eventContent_.getObject()).getDataCopy();
        s3d::Array<nByte> retKeys;
        {
            auto keys = dic.getKeys();
            for (unsigned int i{}; i < keys.getSize(); ++i)
            {
                retKeys.emplace_back(keys[i]);
            }
        }

        for (const nByte& key : retKeys)
        {
            ret.emplace(key, *dic.getValue(key));
        }

        return ret;
    }

    s3d::HashTable<nByte, s3d::String> SivPhoton::getStringDataCopy(const Object& eventContent_)
    {
        s3d::HashTable<nByte, s3d::String> ret;
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, const char*>>(eventContent_.getObject()).getDataCopy();
        s3d::Array<nByte> retKeys;
        {
            auto keys = dic.getKeys();
            for (unsigned int i{}; i < keys.getSize(); ++i)
            {
                retKeys.emplace_back(keys[i]);
            }
        }

        for (const nByte& key : retKeys)
        {
            ret.emplace(key, s3d::Unicode::Widen(*dic.getValue(key)));
        }

        return ret;
    }

    void SivPhoton::debugReturn(int /*debugLevel*/, const s3d::String& /*string*/)
    {
    }

    void SivPhoton::connectionErrorReturn(int /*errorCode*/)
    {
    }

    void SivPhoton::clientErrorReturn(int /*errorCode*/)
    {
    }

    void SivPhoton::warningReturn(int /*warningCode*/)
    {
    }

    void SivPhoton::serverErrorReturn(int /*errorCode*/)
    {
    }

    void SivPhoton::joinRoomEventAction(int /*playerNr*/, const s3d::Array<int>& /*playernrs*/, const Player& /*player*/, const bool /*isSelfEnter*/)
    {
    }

    void SivPhoton::leaveRoomEventAction(int /*playerNr*/, bool /*isInactive*/)
    {
    }

    void SivPhoton::customEventAction(int /*playerNr*/, nByte /*eventCode*/, const Object& /*eventContent*/)
    {
    }

    void SivPhoton::connectReturn(int /*errorCode*/, const s3d::String& /*errorString*/, const s3d::String& /*region*/, const s3d::String& /*cluster*/)
    {
    }

    void SivPhoton::disconnectReturn()
    {
    }

    void SivPhoton::leaveRoomReturn(int /*errorCode*/, const s3d::String& /*errorString*/)
    {
    }

    void SivPhoton::createRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const s3d::String& /*errorString*/)
    {
    }

    void SivPhoton::joinRandomRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const s3d::String& /*errorString*/)
    {
    }
}