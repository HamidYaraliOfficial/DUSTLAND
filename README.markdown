# DUSTLAND

**Open-World Post-Apocalyptic Survival RPG · Colony Builder · Dynamic Ecosystem Simulation**
Built with Unreal Engine 5 and C++.

Language: [English](#english) · [فارسی](#فارسی) · [中文](#中文)

---

<a id="english"></a>
## English

### Overview

Dustland is an original, fully fictional open-world survival RPG project set
in a ruined, independent world of abandoned cities, wastelands, dried lakes,
old mines, and buried settlements. The long-term design targets a
**Persistent World Simulation** where factions, settlements, NPCs, wildlife,
weather, and trade continue evolving whether or not the player is present.

This repository is a **living codebase built incrementally**, not a finished
game. What is described below is split clearly into **Implemented Now** and
**Planned** so nothing here is overstated.

### ✅ Implemented in this build

| System | What it does |
|---|---|
| **Survival Component** | Hunger, Thirst, Fatigue, Energy and Body Temperature meters with configurable depletion/regen rates, difficulty-driven scarcity multiplier, threshold-based status levels (Fine/Stressed/Critical/Failing), and health drain when a stat collapses. |
| **Inventory Component** | Data-driven items (DataTable-based `FDustlandItemDefinition`), weight/volume capacity limits, stacking, transfer between containers (backpack ↔ vehicle ↔ settlement storage ↔ loot), tag-based queries. |
| **Operating Hours Component** | A reusable "is it open right now, and for how much longer" system for Trading Posts, Settlement gates, or Radio broadcast windows. Hours are fully configurable — by a designer in the editor **or by the player/user at runtime** through `SetWindows()` / `AddWindow()` — and the component calculates the live open/closed state and the exact time remaining until the next change itself. |
| **Save Game** | Versioned `USaveGame` object with a schema-migration hook (`MigrateIfNeeded`) and a checksum-based integrity check (`IsIntegrityValid`) that detects corrupted/truncated save files. |
| **World Clock** | Single authoritative day/night clock (current hour, day number, adjustable time scale) with a smooth day/night ambient-temperature curve; every other time-aware system reads from it so nothing drifts out of sync. |
| **Weather System** | Nine weather types (Clear, Dust Storm, Sandstorm, Rain, Heavy Rain, Fog, Heatwave, Cold Snap, Thunderstorm) with weighted-random transitions, per-type duration ranges, temperature modifiers, visibility multipliers and a Survival drain multiplier. |
| **Equipment System** | Slot-based equip/unequip (Head, Mask, Torso, Legs, Feet, Gloves, Backpack, 2 Accessories) that moves items to/from the Inventory component, aggregates insulation/armor/bonus carry capacity, and degrades Condition over time. |
| **Crafting System** | Data-driven recipes (`FDustlandRecipeDefinition`), a `CraftingKnowledgeComponent` for per-actor recipe unlocks, and a `CraftingStationComponent` that validates ingredients/station/unlock state and runs real timed crafting jobs (ingredients consumed up front, output delivered when the timer completes). |
| **GameMode + Difficulty + World Sync** | Five difficulty presets driving the Survival scarcity multiplier, plus a World Manager loop that propagates World Clock hour + Weather temperature into Survival, and Equipment insulation into Survival, and the current hour into every Operating Hours component. |
| **Localization scaffolding** | `Config/DefaultGame.ini` ships English, Persian and Simplified Chinese as staged cultures; `EDustlandTextDirection` gives the UI layer a single source of truth for RTL (Persian) vs LTR (English/Chinese) layout. |
| **Build tooling** | PowerShell scripts for environment validation, Development build, Shipping build, automated tests, packaging and cleaning (see below). |

### 🚧 Planned (design targets, not yet in the codebase)

World Simulation & Faction AI · Settlement construction & population ·
Vehicle physics & garages · Research trees · Quest & Dialogue
graphs · Companion AI · NPC schedules/memory/relationships ·
Wildlife ecosystem & AI archetypes · Scavenging & loot
tables · Trade caravans · Radio network & world news · Modding SDK &
Scenario Editor · Photo Mode · Full UMG/CommonUI theme set (Dark, Light,
Desert Amber, Rust Red, Blue Tech, Green Survival, High Contrast).

Basic Weather, day/night, Equipment and Crafting are now implemented (see
table above); what remains for those specifically is biome-specific weather
tables, multi-stage crafting chains and research-gated recipe unlocks.

See [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md) for the module layout and
design rationale, and the per-system docs referenced there as they land.

### Requirements

- Windows 10/11 (64-bit)
- Unreal Engine 5.4+
- Visual Studio 2022 with **Desktop development with C++** and **Game
  development with C++** workloads
- Windows 10/11 SDK
- Git

### Installation & Quick Start

Full step-by-step instructions: [`INSTALLATION.md`](INSTALLATION.md).

```powershell
# 1. Validate environment + generate Dustland.sln
./Scripts/setup-windows.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.4"

# 2. Build the editor (Development configuration)
./Scripts/build-development.ps1

# 3. Open the project
& "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "Dustland.uproject"
```

### Scripts reference

| Script | Purpose |
|---|---|
| `Scripts/setup-windows.ps1` | Validates Git/VS2022/SDK/UE5, generates `.sln` |
| `Scripts/build-development.ps1` | Compiles `DustlandEditor`, Win64, Development |
| `Scripts/build-shipping.ps1` | Compiles `Dustland`, Win64, Shipping |
| `Scripts/run-tests.ps1` | Runs the automation test suite headlessly |
| `Scripts/package.ps1` | Cooks + packages a distributable build (en/fa/zh-Hans) |
| `Scripts/clean.ps1` | Removes Binaries/Intermediate/DerivedDataCache/Packaged |

### Repository structure

```
Source/Dustland/    Primary module: GameMode, World Manager wiring, entry point
Source/Core/        Shared types (module name: DustlandCore)
Source/Survival/    Hunger/Thirst/Fatigue/Energy/Temperature
Source/Inventory/   Items + containers
Source/Economy/     Operating hours / market state
Source/Save/        Versioned save game
Source/Weather/     World clock + dynamic weather
Source/Equipment/   Equip slots, insulation, armor, condition
Source/Crafting/    Recipes, unlocks, timed crafting stations
Config/             Engine + Game ini, localization culture list
Scripts/            PowerShell build/test/package tooling
Docs/               Architecture and per-system documentation
```

### Contributing

Issues and pull requests are welcome. Please keep gameplay logic in C++ and
reserve Blueprint for animation, UI composition and designer content
scripting, per [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md).

### License

No license has been selected yet for this repository. Add a `LICENSE` file
before distributing binaries or accepting external contributions.

---

<a id="فارسی"></a>
<div dir="rtl" align="right">

## فارسی

### معرفی

Dustland یک پروژهٔ اوپن-ورلد پساآخرالزمانی، کاملاً خیالی و مستقل است که در
جهانی ویران‌شده شامل شهرهای متروکه، بیابان‌ها، دریاچه‌های خشک‌شده، معادن قدیمی
و سکونتگاه‌های مدفون‌شده جریان دارد. هدف بلندمدت طراحی، رسیدن به یک
**شبیه‌سازی پایدار جهان** است که در آن فکشن‌ها، سکونتگاه‌ها، NPCها، حیات‌وحش،
آب‌وهوا و تجارت، صرف‌نظر از حضور بازیکن، به تحول خود ادامه دهند.

این مخزن یک **کدبیس در حال ساخت و توسعهٔ تدریجی** است، نه یک بازی تمام‌شده.
موارد زیر به‌وضوح به دو بخش **پیاده‌سازی‌شده** و **برنامه‌ریزی‌شده** تقسیم
شده‌اند تا هیچ ادعای غیرواقعی مطرح نشود.

### ✅ آنچه در این نسخه پیاده‌سازی شده

| سیستم | عملکرد |
|---|---|
| **سیستم Survival** | متر‌های Hunger، Thirst، Fatigue، Energy و دمای بدن با نرخ‌های مصرف/بازیابی قابل‌تنظیم، ضریب کمبود منابع وابسته به Difficulty، سطوح وضعیت مبتنی بر آستانه (سالم/تحت‌فشار/بحرانی/در حال فروپاشی) و کاهش سلامتی هنگام فروپاشی یک شاخص. |
| **سیستم Inventory** | آیتم‌های Data-Driven (بر پایهٔ DataTable با ساختار `FDustlandItemDefinition`)، محدودیت ظرفیت بر اساس وزن/حجم، Stacking، انتقال بین Containerها (کوله‌پشتی ↔ خودرو ↔ انبار سکونتگاه ↔ غنیمت) و جست‌وجو بر اساس Tag. |
| **سیستم ساعات فعالیت (Operating Hours)** | یک سیستم قابل‌استفادهٔ مجدد برای «آیا الان باز است و چقدر تا تغییر بعدی زمان مانده» برای Trading Post، دروازهٔ سکونتگاه یا بازهٔ پخش رادیویی. ساعات کاملاً قابل‌تنظیم هستند — چه توسط طراح در ادیتور و چه **توسط خود کاربر/بازیکن در زمان اجرا** از طریق توابع `SetWindows()` و `AddWindow()` — و این کامپوننت خودش وضعیت باز/بسته و زمان دقیق باقی‌مانده تا تغییر بعدی را محاسبه می‌کند. |
| **سیستم Save** | یک شیء `USaveGame` نسخه‌بندی‌شده با مکانیزم Migration (`MigrateIfNeeded`) و بررسی صحت مبتنی بر Checksum (`IsIntegrityValid`) برای تشخیص فایل‌های Save خراب یا ناقص. |
| **World Clock** | یک ساعت شب‌وروز واحد و معتبر (ساعت جاری، شمارهٔ روز، مقیاس زمان قابل‌تنظیم) به‌همراه منحنی نرم دمای محیط شب‌وروز؛ تمام سیستم‌های وابسته به زمان از همین منبع می‌خوانند تا هیچ‌کدام از هم عقب یا جلو نیفتند. |
| **سیستم آب‌وهوا (Weather)** | نُه نوع آب‌وهوا (Clear، Dust Storm، Sandstorm، Rain، Heavy Rain، Fog، Heatwave، Cold Snap، Thunderstorm) با انتقال تصادفی وزن‌دار، بازهٔ مدت‌زمان اختصاصی هر نوع، ضریب دما، ضریب دید و ضریب مصرف Survival. |
| **سیستم Equipment** | تجهیز/خارج‌کردن مبتنی بر Slot (سر، ماسک، تنه، پا، پاافزار، دستکش، کوله‌پشتی، دو Accessory) که آیتم را بین Equipment و Inventory جابه‌جا می‌کند، Insulation/Armor/ظرفیت اضافهٔ حمل را جمع می‌زند و Condition را با گذر زمان کاهش می‌دهد. |
| **سیستم Crafting** | Recipeهای Data-Driven (`FDustlandRecipeDefinition`)، یک `CraftingKnowledgeComponent` برای Unlock هر Actor و یک `CraftingStationComponent` که مواد/ایستگاه/Unlock را اعتبارسنجی کرده و Jobهای Craft زمان‌دار واقعی اجرا می‌کند (مصرف فوری مواد، تحویل خروجی پس از پایان تایمر). |
| **GameMode و Difficulty و همگام‌سازی جهان** | پنج پیش‌تنظیم Difficulty که ضریب کمبود منابع Survival را کنترل می‌کنند، به‌همراه یک World Manager Loop که ساعت World Clock و دمای Weather را به Survival، Insulation تجهیزات را به Survival و ساعت جاری را به هر Operating Hours Component منتقل می‌کند. |
| **زیرساخت Localization** | فایل `Config/DefaultGame.ini` سه زبان انگلیسی، فارسی و چینی سادگی‌شده را به‌عنوان Culture ثبت می‌کند؛ `EDustlandTextDirection` یک منبع واحد برای تشخیص RTL (فارسی) در برابر LTR (انگلیسی/چینی) به لایهٔ UI می‌دهد. |
| **ابزارهای Build** | اسکریپت‌های PowerShell برای اعتبارسنجی محیط، Build نوع Development، Build نوع Shipping، تست خودکار، Packaging و Clean (جدول زیر را ببینید). |

### 🚧 برنامه‌ریزی‌شده (هدف طراحی، هنوز در کدبیس نیست)

شبیه‌سازی جهان و هوش مصنوعی فکشن‌ها · ساخت‌وساز و جمعیت سکونتگاه · فیزیک
خودرو و گاراژ · درخت‌های Research · گراف Quest و Dialogue · هوش
مصنوعی همراهان · برنامه/حافظه/روابط NPCها ·
اکوسیستم و آرکی‌تایپ‌های حیات‌وحش · Scavenging و Loot Table · کاروان‌های
تجاری · شبکهٔ رادیویی و اخبار جهان · Modding SDK و Scenario Editor · Photo
Mode · مجموعهٔ کامل تم‌های UMG/CommonUI (تاریک، روشن، کهربایی صحرا، قرمز
زنگ‌زده، آبی تکنولوژیک، سبز بقا، کنتراست بالا).

نسخهٔ پایهٔ Weather، شب‌وروز، Equipment و Crafting اکنون پیاده‌سازی شده‌اند
(جدول بالا را ببینید)؛ آنچه برای این سیستم‌ها باقی مانده شامل جداول آب‌وهوای
مخصوص هر Biome، زنجیرهٔ Crafting چندمرحله‌ای و Unlock مبتنی بر Research است.

برای نقشهٔ ماژول‌ها و منطق طراحی به [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md)
مراجعه کنید.

### پیش‌نیازها

- ویندوز ۱۰/۱۱ (۶۴ بیتی)
- Unreal Engine نسخهٔ ۵.۴ به بالا
- Visual Studio 2022 با Workloadهای **Desktop development with C++** و
  **Game development with C++**
- Windows 10/11 SDK
- Git

### نصب و شروع سریع

راهنمای کامل قدم‌به‌قدم: [`INSTALLATION.md`](INSTALLATION.md)

```powershell
# ۱. اعتبارسنجی محیط + ساخت Dustland.sln
./Scripts/setup-windows.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.4"

# ۲. کامپایل ادیتور (پیکربندی Development)
./Scripts/build-development.ps1

# ۳. باز کردن پروژه
& "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "Dustland.uproject"
```

### مرجع اسکریپت‌ها

| اسکریپت | کاربرد |
|---|---|
| `Scripts/setup-windows.ps1` | اعتبارسنجی Git/VS2022/SDK/UE5 و تولید فایل `.sln` |
| `Scripts/build-development.ps1` | کامپایل `DustlandEditor`، Win64، Development |
| `Scripts/build-shipping.ps1` | کامپایل `Dustland`، Win64، Shipping |
| `Scripts/run-tests.ps1` | اجرای مجموعه تست‌های خودکار به‌صورت Headless |
| `Scripts/package.ps1` | Cook و Package یک نسخهٔ قابل‌توزیع (en/fa/zh-Hans) |
| `Scripts/clean.ps1` | حذف پوشه‌های Binaries/Intermediate/DerivedDataCache/Packaged |

### ساختار مخزن

```
Source/Dustland/    ماژول اصلی: GameMode، اتصال World Manager، نقطهٔ ورود
Source/Core/        انواع مشترک (نام ماژول: DustlandCore)
Source/Survival/    Hunger/Thirst/Fatigue/Energy/Temperature
Source/Inventory/   آیتم‌ها و Containerها
Source/Economy/     ساعات فعالیت / وضعیت بازار
Source/Save/        Save Game نسخه‌بندی‌شده
Source/Weather/     World Clock و آب‌وهوای پویا
Source/Equipment/   Slotهای تجهیزات، Insulation، Armor، Condition
Source/Crafting/    Recipeها، Unlockها، ایستگاه‌های Crafting زمان‌دار
Config/             فایل‌های ini موتور و بازی، فهرست زبان‌ها
Scripts/            ابزارهای PowerShell برای Build/Test/Package
Docs/               مستندات معماری و هر سیستم
```

### مشارکت

ثبت Issue و ارسال Pull Request خوش‌آمد است. لطفاً منطق Gameplay را در C++
نگه دارید و Blueprint را طبق [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md)
فقط برای Animation، ترکیب UI و Scripting محتوای طراحی استفاده کنید.

### مجوز

هنوز مجوزی برای این مخزن انتخاب نشده است. پیش از توزیع فایل‌های اجرایی یا
پذیرش مشارکت‌های خارجی، یک فایل `LICENSE` اضافه کنید.

</div>

---

<a id="中文"></a>
## 中文

### 项目概述

Dustland 是一个完全原创、独立虚构的开放世界末日生存 RPG 项目,背景设定在
一个荒废的世界中,包含废弃城市、荒漠、干涸的湖泊、旧矿井和被掩埋的定居点。
长期设计目标是构建一个**持久世界模拟**系统,让派系、定居点、NPC、野生动物、
天气和贸易无论玩家是否在场都持续演变。

本仓库是一个**正在逐步构建的活跃代码库**,而不是一款已完成的游戏。下文
明确区分**已实现**与**计划中**的内容,不做任何夸大宣称。

### ✅ 本版本已实现的系统

| 系统 | 功能说明 |
|---|---|
| **生存组件 (Survival)** | 饥饿、口渴、疲劳、能量和体温仪表,支持可配置的消耗/恢复速率、由难度驱动的资源稀缺系数、基于阈值的状态等级(良好/紧张/危急/衰竭),以及某项指标衰竭时触发的生命值损耗。 |
| **物品栏组件 (Inventory)** | 数据驱动的物品定义(基于 DataTable 的 `FDustlandItemDefinition`)、重量/体积容量限制、堆叠、容器间转移(背包 ↔ 载具 ↔ 定居点仓库 ↔ 战利品)以及基于标签的查询。 |
| **营业时间组件 (Operating Hours)** | 一个可复用的"现在是否营业、还剩多久变化"系统,适用于交易站、定居点大门或电台广播时段。营业时间完全可配置——既可以由设计师在编辑器中设置,**也可以由玩家/用户在运行时**通过 `SetWindows()` / `AddWindow()` 输入——该组件会自行计算实时的开/关状态以及距离下次变化的精确剩余时间。 |
| **存档系统 (Save Game)** | 带版本号的 `USaveGame` 对象,配有架构迁移钩子(`MigrateIfNeeded`)和基于校验和的完整性检查(`IsIntegrityValid`),可检测损坏或截断的存档文件。 |
| **世界时钟 (World Clock)** | 单一权威的昼夜时钟(当前小时、天数、可调节的时间倍率),配有平滑的昼夜环境温度曲线;所有依赖时间的系统都从这里读取,确保彼此不会失步。 |
| **天气系统 (Weather)** | 九种天气类型(晴朗、沙尘暴、沙暴、下雨、暴雨、雾、热浪、寒潮、雷暴),支持加权随机切换、各类型独立的持续时间范围、温度修正、能见度倍率以及生存消耗倍率。 |
| **装备系统 (Equipment)** | 基于槽位的装备/卸下(头部、面罩、躯干、腿部、脚部、手套、背包、2 个饰品槽),在装备与物品栏之间移动物品,汇总保温值/护甲值/额外负重,并随时间降低耐久度。 |
| **制作系统 (Crafting)** | 数据驱动的配方(`FDustlandRecipeDefinition`)、用于按角色解锁配方的 `CraftingKnowledgeComponent`,以及验证材料/工作站/解锁状态并运行真实计时制作任务的 `CraftingStationComponent`(立即消耗材料,计时结束后交付产出)。 |
| **游戏模式、难度与世界同步** | 五种难度预设驱动生存系统的资源稀缺系数,并由一个世界管理循环将世界时钟的小时数与天气温度传递给生存系统、将装备保温值传递给生存系统、并将当前小时传递给每个营业时间组件。 |
| **本地化基础架构** | `Config/DefaultGame.ini` 已将英语、波斯语和简体中文注册为分阶段文化(Culture);`EDustlandTextDirection` 为 UI 层提供了判断 RTL(波斯语)与 LTR(英语/中文)布局的统一依据。 |
| **构建工具链** | 用于环境验证、Development 构建、Shipping 构建、自动化测试、打包与清理的 PowerShell 脚本(见下表)。 |

### 🚧 计划中(设计目标,尚未写入代码库)

世界模拟与派系 AI · 定居点建造与人口管理 · 载具物理与车库 ·
研究科技树 · 任务与对话图系统 · 同伴 AI · NPC 日程/记忆/关系系统 ·
野生动物生态与 AI 原型 · 搜刮与战利品表 · 商队贸易 · 电台网络
与世界新闻 · 模组 SDK 与场景编辑器 · 摄影模式 · 完整的 UMG/CommonUI 主题
(深色、浅色、沙漠琥珀、锈红、蓝色科技、绿色生存、高对比度)。

基础版天气、昼夜循环、装备与制作系统现已实现(见上表);这些系统仍待完善
的部分包括:特定生物群系的天气表、多阶段制作链,以及基于研究解锁的配方。

模块结构与设计理念请参见 [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md)。

### 系统要求

- Windows 10/11(64 位)
- Unreal Engine 5.4 或更高版本
- Visual Studio 2022,并安装 **Desktop development with C++** 与
  **Game development with C++** 工作负载
- Windows 10/11 SDK
- Git

### 安装与快速开始

完整分步说明请参见 [`INSTALLATION.md`](INSTALLATION.md)。

```powershell
# 1. 验证环境并生成 Dustland.sln
./Scripts/setup-windows.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.4"

# 2. 编译编辑器(Development 配置)
./Scripts/build-development.ps1

# 3. 打开项目
& "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "Dustland.uproject"
```

### 脚本一览

| 脚本 | 用途 |
|---|---|
| `Scripts/setup-windows.ps1` | 验证 Git/VS2022/SDK/UE5,生成 `.sln` 文件 |
| `Scripts/build-development.ps1` | 编译 `DustlandEditor`,Win64,Development |
| `Scripts/build-shipping.ps1` | 编译 `Dustland`,Win64,Shipping |
| `Scripts/run-tests.ps1` | 以无头模式运行自动化测试套件 |
| `Scripts/package.ps1` | Cook 并打包可分发版本(en/fa/zh-Hans) |
| `Scripts/clean.ps1` | 删除 Binaries/Intermediate/DerivedDataCache/Packaged |

### 仓库结构

```
Source/Dustland/    主模块:GameMode、World Manager 关联逻辑、入口点
Source/Core/        共享类型(模块名:DustlandCore)
Source/Survival/    饥饿/口渴/疲劳/能量/体温
Source/Inventory/   物品与容器
Source/Economy/     营业时间 / 市场状态
Source/Save/        带版本号的存档系统
Source/Weather/     世界时钟与动态天气
Source/Equipment/   装备槽位、保温、护甲、耐久度
Source/Crafting/    配方、解锁、计时制作站
Config/             引擎与游戏 ini、本地化文化列表
Scripts/            PowerShell 构建/测试/打包工具
Docs/               架构与各系统文档
```

### 贡献

欢迎提交 Issue 和 Pull Request。请遵循 [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md)
中的规范,将玩法逻辑保留在 C++ 中,蓝图仅用于动画、UI 组合和设计内容脚本。

### 许可证

本仓库尚未选择许可证。在分发可执行文件或接受外部贡献之前,请添加
`LICENSE` 文件。
