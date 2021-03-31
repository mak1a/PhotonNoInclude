#include <Siv3D.hpp>
#include "SivPhoton.hpp"
#include "PhotonClasses.hpp"

namespace EventCode::Game
{
    constexpr nByte putCell = 1;
    constexpr nByte replay = 2;
}

class SamplePhoton : public PSS::SivPhoton {
private:
    enum class SceneState
    {
        Connection,
        Game
    } m_state;

    HashTable<String, String> m_properties;

    bool m_isMasterClient;

    void connectReturn(int errorCode, const String& errorString, const String& region, const String& cluster) override
    {
        if (errorCode) {
            Print << U"接続出来ませんでした";
            return;
        }

        Print << U"接続しました";
        opJoinRandomRoom(m_properties, 2);
    }

    void disconnectReturn() override
    {
        Print << U"切断しました";
    }

    void createRoomReturn(int localPlayerNr, const PSS::Hashtable& roomProperties, const PSS::Hashtable& playerProperties, int errorCode, const String& errorString) override
    {
        if (errorCode) {
            Print << U"部屋を作成出来ませんでした";
            disconnectPhoton();
            return;
        }

        Print << U"部屋を作成しました！";
        m_isMasterClient = true;
        m_turn = Turn::Self;
        m_myCellType = CellState::Maru;
    }

    void joinRandomRoomReturn(int localPlayerNr, const PSS::Hashtable& roomProperties, const PSS::Hashtable& playerProperties, int errorCode, const String& errorString) override
    {
        if (errorCode) {
            Print << U"部屋が見つかりませんでした";

            opCreateRoom(U"", m_properties, 2);

            Print << U"部屋を作成します...";
            return;
        }

        Print << U"部屋に接続しました!";

        m_isMasterClient = false;
        m_turn = Turn::Enemy;
        m_myCellType = CellState::Batsu;

        m_state = SceneState::Game;  // ゲームシーンに進む
    }

    void joinRoomEventAction(int playerNr, const Array<int>& playernrs, const PSS::Player& player, const bool isSelfEnter) override
    {
        if (isSelfEnter)
        {
            return;
        }

        Print << U"対戦相手が入室しました。";
        m_state = SceneState::Game;  // ゲームシーンに進む
    }

    void leaveRoomEventAction(int playerNr, bool isInactive) override
    {
        Print << U"対戦相手が退室しました。";
        m_state = SceneState::Connection;
    }

    void customEventAction(int playerNr, nByte eventCode, const PSS::Object& eventContent) override
    {
        switch (eventCode) {
        case EventCode::Game::putCell: {
            auto dic = getIntDataCopy(eventContent);
            Point coodinate(dic[1], dic[2]);
            const auto cellType{ static_cast<CellState>(dic[3]) };
            EnemyUpdate(coodinate, cellType);
            break;
        }
        case EventCode::Game::replay: {
            auto dic = getBoolDataCopy(eventContent);
            // マスの情報の配列を全て「None」に変更する
            for (auto& cell : m_cells) {
                cell.ChangeState(CellState::None);
            }
            // 手番を戻す
            m_turn = Turn::Enemy;
        }
        default:
            break;
        }
    }


    // ゲームシーンで使用するメンバ変数やメンバ関数

    /// <summary>
    /// マスの状態
    /// </summary>
    enum class [[nodiscard]] CellState {
        None,   // 未入力
        Maru,   // 丸
        Batsu   // バツ
    };

    class Cell {
        RectF m_cell;

        CellState m_state;

        Point m_coodinate;

    public:
        Cell() noexcept = default;

        constexpr Cell(const double xPos_, const double yPos_, const double size_, const Point& coodinate_) noexcept
            : m_cell(xPos_, yPos_, size_)
            , m_state(CellState::None)
            , m_coodinate(coodinate_) {}

        [[nodiscard]] bool IsMouseOver() const {
            return m_cell.mouseOver();
        }

        [[nodiscard]] bool IsLeftClicked() const {
            return m_cell.leftClicked();
        }

        [[nodiscard]] constexpr Vec2 GetCenterPos() const noexcept {
            return m_cell.center();
        }

        [[nodiscard]] constexpr Point GetCoodinate() const noexcept {
            return m_coodinate;
        }

        CellState GetState() const noexcept {
            return m_state;
        }
        void ChangeState(const CellState& state_) {
            m_state = state_;
        }

        [[nodiscard]] bool IsStateNone() const noexcept {
            return m_state == CellState::None;
        }

        Cell Draw(const ColorF& color_) const {
            m_cell.draw(color_);
            return *this;
        }

        Cell DrawFrame(const double thickness_, const ColorF& color_) const {
            m_cell.drawFrame(thickness_, color_);
            return *this;
        }

        void DrawPattern(const double size_, const double thickness_) const {
            switch (m_state) {
            case CellState::Maru:
                Circle(m_cell.center(), size_).drawFrame(thickness_, Palette::Black);
                break;
            case CellState::Batsu:
                Shape2D::Cross(size_, thickness_, m_cell.center()).draw(Palette::Black);
                break;
            default:
                break;
            }
        }
    };

    /// <summary>
        /// 手番
        /// </summary>
    enum class [[nodiscard]] Turn {
        Self,  // 先攻
        Enemy, // 後攻
        Result
    } m_turn;

    // 1マスのサイズ
    static constexpr int32 masuSize = 150;

    // マス目を表示する際の場所の指標
    const RectF m_ban;

    // masuArrayを2次元配列に変換
    Grid<Cell> m_cells; // (3, 3, masuArray);

    CellState m_myCellType;


    bool CheckLine(const Cell& state1_, const Cell& state2_, const Cell& state3_) const noexcept {
        if (state1_.IsStateNone() || state2_.IsStateNone() || state3_.IsStateNone()) {
            return false;
        }

        return (state1_.GetState() == state2_.GetState() && state2_.GetState() == state3_.GetState());
    }

    void CheckGameEnd() {
        if (std::all_of(m_cells.begin(), m_cells.end(), [](const Cell& cell_) {return !cell_.IsStateNone(); })) {
            m_turn = Turn::Result;
            return;
        }

        // 縦3行を調べる
        // trueが返ったら赤線を引く
        for (int y : step(3)) {
            if (CheckLine(m_cells[y][0], m_cells[y][1], m_cells[y][2])) {
                m_turn = Turn::Result;
                return;
            }
        }

        // 横3列を調べる
        // trueが返ったら赤線を引く
        for (int x : step(3)) {
            if (CheckLine(m_cells[0][x], m_cells[1][x], m_cells[2][x])) {
                m_turn = Turn::Result;
                return;
            }
        }

        // 斜め(左上から右下)
        // trueが返ったら赤線を引く
        if (CheckLine(m_cells[0][0], m_cells[1][1], m_cells[2][2])) {
            m_turn = Turn::Result;
            return;
        }

        // 斜め(右上から左下)
        // trueが返ったら赤線を引く
        if (CheckLine(m_cells[2][0], m_cells[1][1], m_cells[0][2])) {
            m_turn = Turn::Result;
            return;
        }
    }

    /// <summary>
    /// 相手に置いたマス目の情報を送る
    /// </summary>
    /// <param name="cell_">置いたマス目</param>
    void SendOpponent(const Cell& cell_) {
        HashTable<nByte, int32> dic;
        dic.emplace<nByte, int32>(1, cell_.GetCoodinate().x);
        dic.emplace<nByte, int32>(2, cell_.GetCoodinate().y);
        dic.emplace<nByte, int32>(3, static_cast<int32>(cell_.GetState()));

        opRaiseEvent(true, dic, EventCode::Game::putCell);
    }

    void SelfUpdate() {
        // マスをクリックしたら手番によって変更する
        for (auto& cell : m_cells) {
            // マスの状態が「None」ではなかったら早期リターン
            // for文の中の処理なのでreturnではなくcontinue
            if (!cell.IsStateNone()) {
                continue;
            }

            // マスをクリックしたら、状態を変更する
            // 手番も変更する
            if (cell.IsLeftClicked()) {
                cell.ChangeState(m_myCellType);
                SendOpponent(cell);
                m_turn = Turn::Enemy;
                CheckGameEnd();
                return;
            }
        }
    }

    void EnemyUpdate(const Point& coodinate_, const CellState& cellType) {
        m_cells[coodinate_].ChangeState(cellType);
        m_turn = Turn::Self;
        CheckGameEnd();
        return;
    }

    void Result() {
        if (!m_isMasterClient) {
            return;
        }
        // ボタンを押したか
        if (SimpleGUI::Button(U"Retry", Scene::CenterF().movedBy(-100, 250), 200)) {
            HashTable<nByte, bool> dic;
            dic.emplace<nByte, bool>(1, true);
            opRaiseEvent(true, dic, EventCode::Game::replay);

            // マスの情報の配列を全て「None」に変更する
            for (auto& cell : m_cells) {
                cell.ChangeState(CellState::None);
            }
            // 手番を先攻に戻す
            m_turn = Turn::Self;
        }
    }

public:
    SamplePhoton(const String& appID_, const String& appVersion_)
        : PSS::SivPhoton(appID_, appVersion_)
        , m_state(SceneState::Connection)
        , m_isMasterClient(false)
        , m_turn(Turn::Enemy)
        , m_myCellType(CellState::Batsu)
        , m_ban(Arg::center(Scene::Center()), masuSize * 3)
    {
        m_properties.emplace(U"gameType", U"photonSample");

        // Photonと接続
        connectPhoton();

        Array<Cell> cells;

        for (int y : step(3)) {
            for (int x : step(3)) {
                cells.emplace_back(m_ban.tl().x + x * masuSize, m_ban.tl().y + y * masuSize, masuSize, Point(x, y));
            }
        }

        m_cells = Grid<Cell>(3, 3, cells);
    }

    void Update()
    {
        service();

        if (m_state != SceneState::Game)
        {
            return;
        }

        switch (m_turn)
        {
        case Turn::Self:
            SelfUpdate();
            break;
        case Turn::Enemy:
            break;
        case Turn::Result:
            Result();
            break;
        default:
            break;
        }
    }

    void Draw() const
    {
        if (m_state != SceneState::Game)
        {
            return;
        }

        for (const auto& cell : m_cells) {
            cell.Draw(Palette::White).DrawFrame(5, Palette::Black);
            if (cell.IsMouseOver()) {
                cell.Draw(ColorF(Palette::Yellow, 0.5));
            }

            cell.DrawPattern(masuSize * 0.4, 5);
        }

        // 縦3行を調べる
        // trueが返ったら赤線を引く
        for (int y : step(3)) {
            if (CheckLine(m_cells[y][0], m_cells[y][1], m_cells[y][2])) {
                Line(m_cells[y][0].GetCenterPos(), m_cells[y][2].GetCenterPos())
                    .stretched(30).draw(4, ColorF(Palette::Red, 0.6));
            }
        }

        // 横3列を調べる
        // trueが返ったら赤線を引く
        for (int x : step(3)) {
            if (CheckLine(m_cells[0][x], m_cells[1][x], m_cells[2][x])) {
                Line(m_cells[0][x].GetCenterPos(), m_cells[2][x].GetCenterPos())
                    .stretched(30).draw(4, ColorF(Palette::Red, 0.6));
            }
        }

        // 斜め(左上から右下)
        // trueが返ったら赤線を引く
        if (CheckLine(m_cells[0][0], m_cells[1][1], m_cells[2][2])) {
            Line(m_cells[0][0].GetCenterPos(), m_cells[2][2].GetCenterPos())
                .stretched(30).draw(4, ColorF(Palette::Red, 0.6));
        }

        // 斜め(右上から左下)
        // trueが返ったら赤線を引く
        if (CheckLine(m_cells[2][0], m_cells[1][1], m_cells[0][2])) {
            Line(m_cells[2][0].GetCenterPos(), m_cells[0][2].GetCenterPos())
                .stretched(30).draw(4, ColorF(Palette::Red, 0.6));
        }
    }
};

void Main() {
    // タイトルを設定
    Window::SetTitle(U"Photonサンプル");

    // ウィンドウの大きさを設定
    Window::Resize(1280, 720);

    // 背景色を設定
    Scene::SetBackground(ColorF(0.2, 0.8, 0.4));


    // 宣言
    SamplePhoton photon{ U"PhotonのappIDを入力してください", U"1.0" };

    while (System::Update()) {
        // Photonの情報をアップデート
        photon.Update();

        photon.Draw();
    }
}
