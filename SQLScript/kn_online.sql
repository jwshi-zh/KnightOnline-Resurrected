if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ACCOUNT_LOGIN]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[ACCOUNT_LOGIN]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_FRI]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_FRI]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_MON]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_MON]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_SAT]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_SAT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_SUN]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_SUN]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_THU]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_THU]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_TUE]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_TUE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_USERDATA]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BACKUP_WED]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[BACKUP_WED]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CHECK_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CHECK_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CREATE_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CREATE_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CREATE_NEW_CHAR]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CREATE_NEW_CHAR]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[DELETE_CHAR]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[DELETE_CHAR]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[DELETE_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[DELETE_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EXEC_SEARCH_USER_ITEM]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[EXEC_SEARCH_USER_ITEM]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LEVEL_CHEAT]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[LEVEL_CHEAT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LOAD_ACCOUNT_CHARID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[LOAD_ACCOUNT_CHARID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LOAD_USER_DATA]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[LOAD_USER_DATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[NATION_SELECT]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[NATION_SELECT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[RANK_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[RANK_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[UPDATE_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[UPDATE_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[UPDATE_USER_DATA]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[UPDATE_USER_DATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[UPDATE_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[UPDATE_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CREATE_KNIGHTS2]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CREATE_KNIGHTS2]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EDITER_KNIGHTS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[EDITER_KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EXEC_KNIGHTS_USER]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[EXEC_KNIGHTS_USER]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EXEC_NPCPOS_SORT]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[EXEC_NPCPOS_SORT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[KNIGHTS_RATING_UPDATE]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[KNIGHTS_RATING_UPDATE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LOAD_CHAR_INFO]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[LOAD_CHAR_INFO]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LOAD_KNIGHTS_MEMBERS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[LOAD_KNIGHTS_MEMBERS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ACCOUNT_CHAR]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ACCOUNT_CHAR]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_FRI_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_FRI_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_FRI_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_FRI_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_MON_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_MON_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_MON_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_MON_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_SAT_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_SAT_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_SAT_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_SAT_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_SUN_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_SUN_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_SUN_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_SUN_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_THU_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_THU_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_THU_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_THU_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_TUE_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_TUE_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_TUE_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_TUE_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_WED_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_WED_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_WED_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_WED_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BLOCKED_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BLOCKED_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BLOCKED_WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BLOCKED_WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[COEFFICIENT]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[COEFFICIENT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[DELETED_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[DELETED_USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EVENT]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[EVENT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[HOME]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[HOME]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ITEM]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ITEM]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ITEM_GROUP]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ITEM_GROUP]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[KNIGHTS]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[KNIGHTS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[KNIGHTS_RATING]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[KNIGHTS_RATING]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[KNIGHTS_USER]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[KNIGHTS_USER]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_MONSTER]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_MONSTER]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_MONSTER_ITEM]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_MONSTER_ITEM]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_NPC]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_NPC]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_NPCPOS]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_NPCPOS]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_NPCPOS_BACK]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_NPCPOS_BACK]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[K_NPCPOS_SORT]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[K_NPCPOS_SORT]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[LEVEL_UP]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[LEVEL_UP]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE1]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE1]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE2]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE2]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE3]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE3]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE4]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE4]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE5]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE5]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAGIC_TYPE8]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAGIC_TYPE8]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAKE_ITEM]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAKE_ITEM]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAKE_ITEM_GRADECODE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAKE_ITEM_GRADECODE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAKE_ITEM_LARECODE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAKE_ITEM_LARECODE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MONSTER_ITEM_TEST]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MONSTER_ITEM_TEST]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[USERDATA]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[WAREHOUSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[WAREHOUSE]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ZONE_INFO]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ZONE_INFO]
GO

if not exists (select * from master.dbo.syslogins where loginname = N'drgame')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'KN_Online', @loginlang = N'한국어'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'drgame', null, @logindb, @loginlang
END
GO

if not exists (select * from master.dbo.syslogins where loginname = N'DRODB7\Administrator')
	exec sp_grantlogin N'DRODB7\Administrator'
	exec sp_defaultdb N'DRODB7\Administrator', N'master'
	exec sp_defaultlanguage N'DRODB7\Administrator', N'한국어'
GO

if not exists (select * from master.dbo.syslogins where loginname = N'knight')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'KN_Online', @loginlang = N'한국어'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'knight', null, @logindb, @loginlang
END
GO

if not exists (select * from master.dbo.syslogins where loginname = N'repent')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'master', @loginlang = N'한국어'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'repent', null, @logindb, @loginlang
END
GO

exec sp_addsrvrolemember N'drgame', sysadmin
GO

exec sp_addsrvrolemember N'DRODB7\Administrator', sysadmin
GO

if not exists (select * from dbo.sysusers where name = N'knight' and uid < 16382)
	EXEC sp_grantdbaccess N'knight', N'knight'
GO

exec sp_addrolemember N'db_owner', N'knight'
GO

CREATE TABLE [dbo].[ACCOUNT_CHAR] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[bNation] [tinyint] NOT NULL ,
	[bCharNum] [tinyint] NOT NULL ,
	[strCharID1] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[strCharID2] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[strCharID3] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[strCharID4] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[strCharID5] [char] (21) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_FRI_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_FRI_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_MON_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_MON_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_SAT_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_SAT_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_SUN_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_SUN_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_THU_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_THU_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_TUE_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_TUE_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_WED_USERDATA] (
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BK_WED_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[warehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BLOCKED_USERDATA] (
	[strUserId] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Nation] [tinyint] NOT NULL ,
	[Race] [tinyint] NOT NULL ,
	[Class] [smallint] NOT NULL ,
	[HairColor] [tinyint] NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Face] [tinyint] NOT NULL ,
	[City] [tinyint] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Hp] [smallint] NOT NULL ,
	[Mp] [smallint] NOT NULL ,
	[Sp] [smallint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Authority] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[Zone] [tinyint] NOT NULL ,
	[Bind] [smallint] NULL ,
	[PX] [int] NOT NULL ,
	[PZ] [int] NOT NULL ,
	[PY] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BLOCKED_WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[WarehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[COEFFICIENT] (
	[sClass] [smallint] NOT NULL ,
	[ShortSword] [float] NOT NULL ,
	[Sword] [float] NOT NULL ,
	[Axe] [float] NOT NULL ,
	[Club] [float] NOT NULL ,
	[Spear] [float] NOT NULL ,
	[Pole] [float] NOT NULL ,
	[Staff] [float] NOT NULL ,
	[Bow] [float] NULL ,
	[Hp] [float] NOT NULL ,
	[Mp] [float] NOT NULL ,
	[Sp] [float] NOT NULL ,
	[Ac] [float] NOT NULL ,
	[Hitrate] [float] NOT NULL ,
	[Evasionrate] [float] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[DELETED_USERDATA] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strUserId] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Nation] [tinyint] NOT NULL ,
	[Race] [tinyint] NOT NULL ,
	[Class] [smallint] NOT NULL ,
	[HairColor] [tinyint] NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Face] [tinyint] NOT NULL ,
	[City] [tinyint] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Hp] [smallint] NOT NULL ,
	[Mp] [smallint] NOT NULL ,
	[Sp] [smallint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Authority] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[Zone] [tinyint] NOT NULL ,
	[Bind] [smallint] NULL ,
	[PX] [int] NOT NULL ,
	[PZ] [int] NULL ,
	[PY] [int] NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL ,
	[DeletedTime] [smalldatetime] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[EVENT] (
	[ZoneNum] [tinyint] NOT NULL ,
	[EventNum] [smallint] NOT NULL ,
	[Type] [tinyint] NOT NULL ,
	[Cond1] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Cond2] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Cond3] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Cond4] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Cond5] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Exec1] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Exec2] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Exec3] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Exec4] [char] (128) COLLATE Korean_Wansung_CI_AS NULL ,
	[Exec5] [char] (128) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[HOME] (
	[Nation] [tinyint] NOT NULL ,
	[ElmoZoneX] [int] NOT NULL ,
	[ElmoZoneZ] [int] NOT NULL ,
	[ElmoZoneLX] [tinyint] NOT NULL ,
	[ElmoZoneLZ] [tinyint] NOT NULL ,
	[KarusZoneX] [int] NOT NULL ,
	[KarusZoneZ] [int] NOT NULL ,
	[KarusZoneLX] [tinyint] NOT NULL ,
	[KarusZoneLZ] [tinyint] NOT NULL ,
	[FreeZoneX] [int] NOT NULL ,
	[FreeZoneZ] [int] NOT NULL ,
	[FreeZoneLX] [tinyint] NOT NULL ,
	[FreeZoneLZ] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[ITEM] (
	[Num] [int] NOT NULL ,
	[strName] [char] (50) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Kind] [tinyint] NOT NULL ,
	[Slot] [tinyint] NOT NULL ,
	[Race] [tinyint] NOT NULL ,
	[Class] [tinyint] NOT NULL ,
	[Damage] [smallint] NOT NULL ,
	[Delay] [smallint] NOT NULL ,
	[Range] [smallint] NOT NULL ,
	[Weight] [smallint] NOT NULL ,
	[Duration] [smallint] NOT NULL ,
	[BuyPrice] [int] NOT NULL ,
	[SellPrice] [int] NOT NULL ,
	[Ac] [smallint] NOT NULL ,
	[Countable] [tinyint] NOT NULL ,
	[Effect1] [int] NOT NULL ,
	[Effect2] [int] NOT NULL ,
	[ReqLevel] [tinyint] NOT NULL ,
	[ReqRank] [tinyint] NOT NULL ,
	[ReqTitle] [tinyint] NOT NULL ,
	[ReqStr] [tinyint] NOT NULL ,
	[ReqSta] [tinyint] NOT NULL ,
	[ReqDex] [tinyint] NOT NULL ,
	[ReqIntel] [tinyint] NOT NULL ,
	[ReqCha] [tinyint] NOT NULL ,
	[SellingGroup] [tinyint] NOT NULL ,
	[ItemType] [tinyint] NOT NULL ,
	[Hitrate] [smallint] NOT NULL ,
	[Evasionrate] [smallint] NOT NULL ,
	[DaggerAc] [smallint] NOT NULL ,
	[SwordAc] [smallint] NOT NULL ,
	[MaceAc] [smallint] NOT NULL ,
	[AxeAc] [smallint] NOT NULL ,
	[SpearAc] [smallint] NOT NULL ,
	[BowAc] [smallint] NOT NULL ,
	[FireDamage] [tinyint] NOT NULL ,
	[IceDamage] [tinyint] NOT NULL ,
	[LightningDamage] [tinyint] NOT NULL ,
	[PoisonDamage] [tinyint] NOT NULL ,
	[HPDrain] [tinyint] NOT NULL ,
	[MPDamage] [tinyint] NOT NULL ,
	[MPDrain] [tinyint] NOT NULL ,
	[MirrorDamage] [tinyint] NOT NULL ,
	[Droprate] [tinyint] NOT NULL ,
	[StrB] [smallint] NOT NULL ,
	[StaB] [smallint] NOT NULL ,
	[DexB] [smallint] NOT NULL ,
	[IntelB] [smallint] NOT NULL ,
	[ChaB] [smallint] NOT NULL ,
	[MaxHpB] [smallint] NOT NULL ,
	[MaxMpB] [smallint] NOT NULL ,
	[FireR] [smallint] NOT NULL ,
	[ColdR] [smallint] NOT NULL ,
	[LightningR] [smallint] NOT NULL ,
	[MagicR] [smallint] NOT NULL ,
	[PoisonR] [smallint] NOT NULL ,
	[CurseR] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[ITEM_GROUP] (
	[group] [smallint] NOT NULL ,
	[name] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[item1] [int] NOT NULL ,
	[item2] [int] NOT NULL ,
	[item3] [int] NOT NULL ,
	[item4] [int] NOT NULL ,
	[item5] [int] NOT NULL ,
	[item6] [int] NOT NULL ,
	[item7] [int] NOT NULL ,
	[item8] [int] NOT NULL ,
	[item9] [int] NOT NULL ,
	[item10] [int] NOT NULL ,
	[item11] [int] NOT NULL ,
	[item12] [int] NOT NULL ,
	[item13] [int] NOT NULL ,
	[item14] [int] NOT NULL ,
	[item15] [int] NOT NULL ,
	[item16] [int] NOT NULL ,
	[item17] [int] NOT NULL ,
	[item18] [int] NOT NULL ,
	[item19] [int] NOT NULL ,
	[item20] [int] NOT NULL ,
	[item21] [int] NOT NULL ,
	[item22] [int] NOT NULL ,
	[item23] [int] NOT NULL ,
	[item24] [int] NOT NULL ,
	[item25] [int] NOT NULL ,
	[item26] [int] NOT NULL ,
	[item27] [int] NOT NULL ,
	[item28] [int] NOT NULL ,
	[item29] [int] NOT NULL ,
	[item30] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[KNIGHTS] (
	[IDNum] [smallint] NOT NULL ,
	[Flag] [tinyint] NOT NULL ,
	[Nation] [tinyint] NOT NULL ,
	[Ranking] [tinyint] NOT NULL ,
	[IDName] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Members] [smallint] NOT NULL ,
	[Chief] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[ViceChief_1] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[ViceChief_2] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[ViceChief_3] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[Gold] [bigint] NOT NULL ,
	[Domination] [smallint] NOT NULL ,
	[Points] [int] NULL ,
	[CreateTime] [smalldatetime] NOT NULL ,
	[Mark] [image] NULL ,
	[Stash] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[KNIGHTS_RATING] (
	[nRank] [int] IDENTITY (1, 1) NOT NULL ,
	[shIndex] [smallint] NULL ,
	[strName] [char] (21) COLLATE Korean_Wansung_CI_AS NULL ,
	[nPoints] [int] NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[KNIGHTS_USER] (
	[sIDNum] [smallint] NOT NULL ,
	[strUserID] [char] (21) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_MONSTER] (
	[sSid] [smallint] NOT NULL ,
	[strName] [varchar] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[sPid] [smallint] NOT NULL ,
	[sSize] [smallint] NOT NULL ,
	[iWeapon1] [int] NOT NULL ,
	[iWeapon2] [int] NOT NULL ,
	[byGroup] [tinyint] NOT NULL ,
	[byActType] [tinyint] NOT NULL ,
	[byType] [tinyint] NOT NULL ,
	[byFamily] [tinyint] NOT NULL ,
	[byRank] [tinyint] NOT NULL ,
	[byTitle] [tinyint] NOT NULL ,
	[iSellingGroup] [int] NOT NULL ,
	[sLevel] [smallint] NOT NULL ,
	[iExp] [int] NOT NULL ,
	[iLoyalty] [int] NOT NULL ,
	[sHpPoint] [smallint] NOT NULL ,
	[sMpPoint] [smallint] NOT NULL ,
	[sAtk] [smallint] NOT NULL ,
	[sAc] [smallint] NOT NULL ,
	[sHitRate] [smallint] NOT NULL ,
	[sEvadeRate] [smallint] NOT NULL ,
	[sDamage] [smallint] NOT NULL ,
	[sAttackDelay] [smallint] NOT NULL ,
	[bySpeed1] [tinyint] NOT NULL ,
	[bySpeed2] [tinyint] NOT NULL ,
	[sStandtime] [smallint] NOT NULL ,
	[iMagic1] [int] NOT NULL ,
	[iMagic2] [int] NOT NULL ,
	[iMagic3] [int] NOT NULL ,
	[byFireR] [tinyint] NOT NULL ,
	[byColdR] [tinyint] NOT NULL ,
	[byLightningR] [tinyint] NOT NULL ,
	[byMagicR] [tinyint] NOT NULL ,
	[byDiseaseR] [tinyint] NOT NULL ,
	[byPoisonR] [tinyint] NOT NULL ,
	[byLightR] [tinyint] NOT NULL ,
	[sBulk] [smallint] NOT NULL ,
	[byAttackRange] [tinyint] NOT NULL ,
	[bySearchRange] [tinyint] NOT NULL ,
	[byTracingRange] [tinyint] NOT NULL ,
	[iMoney] [int] NOT NULL ,
	[sItem] [smallint] NOT NULL ,
	[byDirectAttack] [tinyint] NOT NULL ,
	[byMagicAttack] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_MONSTER_ITEM] (
	[sIndex] [smallint] NOT NULL ,
	[iItem01] [int] NULL ,
	[sPersent01] [smallint] NULL ,
	[iItem02] [int] NULL ,
	[sPersent02] [smallint] NULL ,
	[iItem03] [int] NULL ,
	[sPersent03] [smallint] NULL ,
	[iItem04] [int] NULL ,
	[sPersent04] [smallint] NULL ,
	[iItem05] [int] NULL ,
	[sPersent05] [smallint] NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_NPC] (
	[sSid] [smallint] NOT NULL ,
	[strName] [varchar] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[sPid] [smallint] NOT NULL ,
	[sSize] [smallint] NOT NULL ,
	[iWeapon1] [int] NOT NULL ,
	[iWeapon2] [int] NOT NULL ,
	[byGroup] [tinyint] NOT NULL ,
	[byActType] [tinyint] NOT NULL ,
	[byType] [tinyint] NOT NULL ,
	[byFamily] [tinyint] NOT NULL ,
	[byRank] [tinyint] NOT NULL ,
	[byTitle] [tinyint] NOT NULL ,
	[iSellingGroup] [int] NOT NULL ,
	[sLevel] [smallint] NOT NULL ,
	[iExp] [int] NOT NULL ,
	[iLoyalty] [int] NOT NULL ,
	[sHpPoint] [smallint] NOT NULL ,
	[sMpPoint] [smallint] NOT NULL ,
	[sAtk] [smallint] NOT NULL ,
	[sAc] [smallint] NOT NULL ,
	[sHitRate] [smallint] NOT NULL ,
	[sEvadeRate] [smallint] NOT NULL ,
	[sDamage] [smallint] NOT NULL ,
	[sAttackDelay] [smallint] NOT NULL ,
	[bySpeed1] [tinyint] NOT NULL ,
	[bySpeed2] [tinyint] NOT NULL ,
	[sStandtime] [smallint] NOT NULL ,
	[iMagic1] [int] NOT NULL ,
	[iMagic2] [int] NOT NULL ,
	[iMagic3] [int] NOT NULL ,
	[byFireR] [tinyint] NOT NULL ,
	[byColdR] [tinyint] NOT NULL ,
	[byLightningR] [tinyint] NOT NULL ,
	[byMagicR] [tinyint] NOT NULL ,
	[byDiseaseR] [tinyint] NOT NULL ,
	[byPoisonR] [tinyint] NOT NULL ,
	[byLightR] [tinyint] NOT NULL ,
	[sBulk] [smallint] NOT NULL ,
	[byAttackRange] [tinyint] NOT NULL ,
	[bySearchRange] [tinyint] NOT NULL ,
	[byTracingRange] [tinyint] NOT NULL ,
	[iMoney] [int] NOT NULL ,
	[sItem] [smallint] NOT NULL ,
	[byDirectAttack] [tinyint] NOT NULL ,
	[byMagicAttack] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_NPCPOS] (
	[ZoneID] [smallint] NOT NULL ,
	[NpcID] [int] NOT NULL ,
	[ActType] [tinyint] NOT NULL ,
	[LeftX] [int] NOT NULL ,
	[TopZ] [int] NOT NULL ,
	[RightX] [int] NOT NULL ,
	[BottomZ] [int] NOT NULL ,
	[NumNPC] [tinyint] NOT NULL ,
	[RegTime] [smallint] NOT NULL ,
	[DotCnt] [tinyint] NOT NULL ,
	[path] [text] COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_NPCPOS_BACK] (
	[ZoneID] [smallint] NOT NULL ,
	[NpcID] [int] NOT NULL ,
	[ActType] [tinyint] NOT NULL ,
	[LeftX] [int] NOT NULL ,
	[TopZ] [int] NOT NULL ,
	[RightX] [int] NOT NULL ,
	[BottomZ] [int] NOT NULL ,
	[NumNPC] [tinyint] NOT NULL ,
	[RegTime] [smallint] NOT NULL ,
	[DotCnt] [tinyint] NOT NULL ,
	[path] [text] COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[K_NPCPOS_SORT] (
	[ZoneID] [smallint] NOT NULL ,
	[NpcID] [int] NOT NULL ,
	[ActType] [tinyint] NOT NULL ,
	[LeftX] [int] NOT NULL ,
	[TopZ] [int] NOT NULL ,
	[RightX] [int] NOT NULL ,
	[BottomZ] [int] NOT NULL ,
	[NumNPC] [tinyint] NOT NULL ,
	[RegTime] [smallint] NOT NULL ,
	[DotCnt] [tinyint] NOT NULL ,
	[path] [text] COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[LEVEL_UP] (
	[level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC] (
	[MagicNum] [int] NOT NULL ,
	[EnName] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[KrName] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[BeforeAction] [tinyint] NOT NULL ,
	[TargetAction] [tinyint] NOT NULL ,
	[SelfEffect] [tinyint] NOT NULL ,
	[FlyingEffect] [tinyint] NOT NULL ,
	[TargetEffect] [smallint] NOT NULL ,
	[Moral] [tinyint] NOT NULL ,
	[SkillLevel] [smallint] NOT NULL ,
	[Skill] [smallint] NOT NULL ,
	[Msp] [smallint] NOT NULL ,
	[HP] [smallint] NOT NULL ,
	[ItemGroup] [tinyint] NOT NULL ,
	[UseItem] [int] NOT NULL ,
	[CastTime] [tinyint] NOT NULL ,
	[ReCastTime] [tinyint] NOT NULL ,
	[SuccessRate] [tinyint] NOT NULL ,
	[Type1] [tinyint] NOT NULL ,
	[Type2] [tinyint] NOT NULL ,
	[Range] [smallint] NOT NULL ,
	[Etc] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE1] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[Type] [tinyint] NOT NULL ,
	[HitRate] [smallint] NOT NULL ,
	[Hit] [smallint] NOT NULL ,
	[Delay] [tinyint] NOT NULL ,
	[ComboType] [tinyint] NOT NULL ,
	[ComboCount] [tinyint] NOT NULL ,
	[ComboDamage] [smallint] NOT NULL ,
	[Range] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE2] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[HitType] [tinyint] NOT NULL ,
	[HitRate] [smallint] NOT NULL ,
	[AddDamage] [smallint] NOT NULL ,
	[AddRange] [smallint] NOT NULL ,
	[NeedArrow] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE3] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[Radius] [tinyint] NOT NULL ,
	[Angle] [smallint] NOT NULL ,
	[DirectType] [tinyint] NOT NULL ,
	[FirstDamage] [smallint] NOT NULL ,
	[EndDamage] [smallint] NOT NULL ,
	[TimeDamage] [smallint] NOT NULL ,
	[Duration] [tinyint] NOT NULL ,
	[Attribute] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE4] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[BuffType] [tinyint] NOT NULL ,
	[Radius] [tinyint] NOT NULL ,
	[Duration] [smallint] NOT NULL ,
	[AttackSpeed] [tinyint] NOT NULL ,
	[Speed] [tinyint] NOT NULL ,
	[AC] [smallint] NOT NULL ,
	[Attack] [tinyint] NOT NULL ,
	[MaxHP] [smallint] NOT NULL ,
	[HitRate] [tinyint] NOT NULL ,
	[AvoidRate] [smallint] NOT NULL ,
	[Str] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[FireR] [tinyint] NOT NULL ,
	[ColdR] [tinyint] NOT NULL ,
	[LightningR] [tinyint] NOT NULL ,
	[MagicR] [tinyint] NOT NULL ,
	[DiseaseR] [tinyint] NOT NULL ,
	[PoisonR] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE5] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[Type] [tinyint] NOT NULL ,
	[ExpRecover] [tinyint] NOT NULL ,
	[NeedStone] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAGIC_TYPE8] (
	[iNum] [int] NOT NULL ,
	[Name] [char] (30) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Description] [char] (100) COLLATE Korean_Wansung_CI_AS NULL ,
	[Target] [tinyint] NOT NULL ,
	[Radius] [smallint] NOT NULL ,
	[WarpType] [tinyint] NOT NULL ,
	[ExpRecover] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAKE_ITEM] (
	[sIndex] [smallint] NOT NULL ,
	[strItemInfo] [char] (20) COLLATE Korean_Wansung_CI_AS NULL ,
	[iItemCode] [int] NOT NULL ,
	[byItemLevel] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAKE_ITEM_GRADECODE] (
	[byItemIndex] [tinyint] NOT NULL ,
	[byGrade_1] [tinyint] NOT NULL ,
	[byGrade_2] [tinyint] NULL ,
	[byGrade_3] [tinyint] NULL ,
	[byGrade_4] [tinyint] NULL ,
	[byGrade_5] [tinyint] NULL ,
	[byGrade_6] [tinyint] NULL ,
	[byGrade_7] [tinyint] NULL ,
	[byGrade_8] [tinyint] NULL ,
	[byGrade_9] [tinyint] NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MAKE_ITEM_LARECODE] (
	[byLevelGrade] [tinyint] NOT NULL ,
	[sLareItem] [smallint] NOT NULL ,
	[sMagicItem] [smallint] NOT NULL ,
	[sGereralItem] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[MONSTER_ITEM_TEST] (
	[sIndex] [smallint] NOT NULL ,
	[iItem01] [int] NULL ,
	[sPersent01] [smallint] NULL ,
	[iItem02] [int] NULL ,
	[sPersent02] [smallint] NULL ,
	[iItem03] [int] NULL ,
	[sPersent03] [smallint] NULL ,
	[iItem04] [int] NULL ,
	[sPersent04] [smallint] NULL ,
	[iItem05] [int] NULL ,
	[sPersent05] [smallint] NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[USERDATA] (
	[strUserId] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[Nation] [tinyint] NOT NULL ,
	[Race] [tinyint] NOT NULL ,
	[Class] [smallint] NOT NULL ,
	[HairColor] [tinyint] NOT NULL ,
	[Rank] [tinyint] NOT NULL ,
	[Title] [tinyint] NOT NULL ,
	[Level] [tinyint] NOT NULL ,
	[Exp] [int] NOT NULL ,
	[Loyalty] [int] NOT NULL ,
	[Face] [tinyint] NOT NULL ,
	[City] [tinyint] NOT NULL ,
	[Knights] [smallint] NOT NULL ,
	[Fame] [tinyint] NOT NULL ,
	[Hp] [smallint] NOT NULL ,
	[Mp] [smallint] NOT NULL ,
	[Sp] [smallint] NOT NULL ,
	[Strong] [tinyint] NOT NULL ,
	[Sta] [tinyint] NOT NULL ,
	[Dex] [tinyint] NOT NULL ,
	[Intel] [tinyint] NOT NULL ,
	[Cha] [tinyint] NOT NULL ,
	[Authority] [tinyint] NOT NULL ,
	[Points] [tinyint] NOT NULL ,
	[Gold] [int] NOT NULL ,
	[Zone] [tinyint] NOT NULL ,
	[Bind] [smallint] NULL ,
	[PX] [int] NOT NULL ,
	[PZ] [int] NOT NULL ,
	[PY] [int] NOT NULL ,
	[dwTime] [int] NOT NULL ,
	[strSkill] [varchar] (10) COLLATE Korean_Wansung_CI_AS NULL ,
	[strItem] [varchar] (400) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[WAREHOUSE] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[nMoney] [int] NOT NULL ,
	[dwTime] [int] NOT NULL ,
	[WarehouseData] [varchar] (1600) COLLATE Korean_Wansung_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[ZONE_INFO] (
	[ServerNo] [tinyint] NOT NULL ,
	[ZoneNo] [smallint] NOT NULL ,
	[strZoneName] [char] (30) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[InitX] [int] NOT NULL ,
	[InitZ] [int] NOT NULL ,
	[InitY] [int] NOT NULL ,
	[Type] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[ACCOUNT_CHAR] WITH NOCHECK ADD 
	CONSTRAINT [PK_ACCOUNT_CHAR] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_FRI_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_FRI_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_FRI_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_FRI_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_MON_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_MON_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_MON_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_MON_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_SAT_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_SAT_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_SAT_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_SAT_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_SUN_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_SUN_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_SUN_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_SUN_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_THU_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_THU_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_THU_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_THU_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_TUE_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_TUE_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_TUE_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_TUE_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_WED_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_WED_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BK_WED_WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_WED_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[ITEM] WITH NOCHECK ADD 
	CONSTRAINT [PK_ITEM] PRIMARY KEY  CLUSTERED 
	(
		[Num]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[KNIGHTS] WITH NOCHECK ADD 
	CONSTRAINT [PK_KNIGHTS] PRIMARY KEY  CLUSTERED 
	(
		[IDNum]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[MAGIC] WITH NOCHECK ADD 
	CONSTRAINT [PK_MAGIC] PRIMARY KEY  CLUSTERED 
	(
		[MagicNum]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[MAGIC_TYPE1] WITH NOCHECK ADD 
	CONSTRAINT [PK_MAGIC_TYPE1] PRIMARY KEY  CLUSTERED 
	(
		[iNum]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[MAGIC_TYPE2] WITH NOCHECK ADD 
	CONSTRAINT [PK_MAGIC_TYPE2] PRIMARY KEY  CLUSTERED 
	(
		[iNum]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[MAGIC_TYPE3] WITH NOCHECK ADD 
	CONSTRAINT [PK_MAGIC_TYPE3] PRIMARY KEY  CLUSTERED 
	(
		[iNum]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserId]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [PK_WAREHOUSE] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[ACCOUNT_CHAR] WITH NOCHECK ADD 
	CONSTRAINT [DF_ACCOUNT_CHAR_bCharNum] DEFAULT (0) FOR [bCharNum]
GO

ALTER TABLE [dbo].[DELETED_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [DF_DELETED_USERDATA_PY] DEFAULT (0) FOR [PY],
	CONSTRAINT [DF_DELETED_USERDATA_DeletedTime] DEFAULT (getdate()) FOR [DeletedTime]
GO

ALTER TABLE [dbo].[ITEM] WITH NOCHECK ADD 
	CONSTRAINT [DF_아이템테이블$_ㅇ] DEFAULT (0) FOR [Kind],
	CONSTRAINT [DF_아이템테이블$_d] DEFAULT (0) FOR [Slot],
	CONSTRAINT [DF_아이템테이블$_종족] DEFAULT (0) FOR [Race],
	CONSTRAINT [DF_ITEM_Class_1] DEFAULT (1) FOR [Class],
	CONSTRAINT [DF_ITEM_Damage_1] DEFAULT (0) FOR [Damage],
	CONSTRAINT [DF_ITEM_Delay_1] DEFAULT (0) FOR [Delay],
	CONSTRAINT [DF_ITEM_Range_1] DEFAULT (0) FOR [Range],
	CONSTRAINT [DF_ITEM_Weight_1] DEFAULT (0) FOR [Weight],
	CONSTRAINT [DF_ITEM_Price_1] DEFAULT (0) FOR [BuyPrice],
	CONSTRAINT [DF_ITEM_Ac_1] DEFAULT (0) FOR [Ac],
	CONSTRAINT [DF_ITEM_Ac1] DEFAULT (0) FOR [DaggerAc],
	CONSTRAINT [DF_ITEM_Ac2] DEFAULT (0) FOR [SwordAc],
	CONSTRAINT [DF_ITEM_Ac3] DEFAULT (0) FOR [MaceAc],
	CONSTRAINT [DF_ITEM_Ac4] DEFAULT (0) FOR [AxeAc],
	CONSTRAINT [DF_ITEM_Ac5] DEFAULT (0) FOR [SpearAc],
	CONSTRAINT [DF_ITEM_Ac6] DEFAULT (0) FOR [BowAc],
	CONSTRAINT [DF_ITEM_Magic_1] DEFAULT (0) FOR [MirrorDamage],
	CONSTRAINT [DF_ITEM_Soul_1] DEFAULT (0) FOR [Droprate],
	CONSTRAINT [DF_ITEM_Str_1] DEFAULT (0) FOR [StrB],
	CONSTRAINT [DF_ITEM_Sta_1] DEFAULT (0) FOR [StaB],
	CONSTRAINT [DF_ITEM_Dex_1] DEFAULT (0) FOR [DexB],
	CONSTRAINT [DF_ITEM_Intel_1] DEFAULT (0) FOR [IntelB],
	CONSTRAINT [DF_ITEM_Charm_1] DEFAULT (0) FOR [ChaB],
	CONSTRAINT [DF_ITEM_FireR_1] DEFAULT (0) FOR [FireR],
	CONSTRAINT [DF_ITEM_ColdR_1] DEFAULT (0) FOR [ColdR],
	CONSTRAINT [DF_ITEM_LightR_1] DEFAULT (0) FOR [LightningR],
	CONSTRAINT [DF_ITEM_MagicR_1] DEFAULT (0) FOR [MagicR],
	CONSTRAINT [DF_ITEM_PoisonR_1] DEFAULT (0) FOR [PoisonR],
	CONSTRAINT [DF_ITEM_DeseaseR] DEFAULT (0) FOR [CurseR]
GO

ALTER TABLE [dbo].[ITEM_GROUP] WITH NOCHECK ADD 
	CONSTRAINT [DF_ITEM_GROUP_item1] DEFAULT (0) FOR [item1],
	CONSTRAINT [DF_ITEM_GROUP_item2] DEFAULT (0) FOR [item2],
	CONSTRAINT [DF_ITEM_GROUP_item3] DEFAULT (0) FOR [item3],
	CONSTRAINT [DF_ITEM_GROUP_item4] DEFAULT (0) FOR [item4],
	CONSTRAINT [DF_ITEM_GROUP_item5] DEFAULT (0) FOR [item5],
	CONSTRAINT [DF_ITEM_GROUP_item6] DEFAULT (0) FOR [item6],
	CONSTRAINT [DF_ITEM_GROUP_item7] DEFAULT (0) FOR [item7],
	CONSTRAINT [DF_ITEM_GROUP_item8] DEFAULT (0) FOR [item8],
	CONSTRAINT [DF_ITEM_GROUP_item9] DEFAULT (0) FOR [item9],
	CONSTRAINT [DF_ITEM_GROUP_item10] DEFAULT (0) FOR [item10],
	CONSTRAINT [DF_ITEM_GROUP_item11] DEFAULT (0) FOR [item11],
	CONSTRAINT [DF_ITEM_GROUP_item12] DEFAULT (0) FOR [item12],
	CONSTRAINT [DF_ITEM_GROUP_item13] DEFAULT (0) FOR [item13],
	CONSTRAINT [DF_ITEM_GROUP_item14] DEFAULT (0) FOR [item14],
	CONSTRAINT [DF_ITEM_GROUP_item15] DEFAULT (0) FOR [item15],
	CONSTRAINT [DF_ITEM_GROUP_item16] DEFAULT (0) FOR [item16],
	CONSTRAINT [DF_ITEM_GROUP_item17] DEFAULT (0) FOR [item17],
	CONSTRAINT [DF_ITEM_GROUP_item18] DEFAULT (0) FOR [item18],
	CONSTRAINT [DF_ITEM_GROUP_item19] DEFAULT (0) FOR [item19],
	CONSTRAINT [DF_ITEM_GROUP_item20] DEFAULT (0) FOR [item20],
	CONSTRAINT [DF_ITEM_GROUP_item21] DEFAULT (0) FOR [item21],
	CONSTRAINT [DF_ITEM_GROUP_item22] DEFAULT (0) FOR [item22],
	CONSTRAINT [DF_ITEM_GROUP_item23] DEFAULT (0) FOR [item23],
	CONSTRAINT [DF_ITEM_GROUP_item24] DEFAULT (0) FOR [item24],
	CONSTRAINT [DF_ITEM_GROUP_item25] DEFAULT (0) FOR [item25],
	CONSTRAINT [DF_ITEM_GROUP_item26] DEFAULT (0) FOR [item26],
	CONSTRAINT [DF_ITEM_GROUP_item27] DEFAULT (0) FOR [item27],
	CONSTRAINT [DF_ITEM_GROUP_item28] DEFAULT (0) FOR [item28],
	CONSTRAINT [DF_ITEM_GROUP_item29] DEFAULT (0) FOR [item29],
	CONSTRAINT [DF_ITEM_GROUP_item30] DEFAULT (0) FOR [item30]
GO

ALTER TABLE [dbo].[KNIGHTS] WITH NOCHECK ADD 
	CONSTRAINT [DF_KNIGHTS_Flag] DEFAULT (1) FOR [Flag],
	CONSTRAINT [DF_KNIGHTS_Ranking] DEFAULT (0) FOR [Ranking],
	CONSTRAINT [DF_KNIGHTS_Members] DEFAULT (1) FOR [Members],
	CONSTRAINT [DF_KNIGHTS_Gold] DEFAULT (0) FOR [Gold],
	CONSTRAINT [DF_KNIGHTS_Domination] DEFAULT (0) FOR [Domination],
	CONSTRAINT [DF_KNIGHTS_Points] DEFAULT (0) FOR [Points],
	CONSTRAINT [DF_KNIGHTS_CreateTime] DEFAULT (getdate()) FOR [CreateTime],
	CONSTRAINT [IX_KNIGHTS] UNIQUE  NONCLUSTERED 
	(
		[IDName]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[K_MONSTER] WITH NOCHECK ADD 
	CONSTRAINT [DF_MONSTER_Class] DEFAULT (0) FOR [iWeapon2],
	CONSTRAINT [DF_MONSTER_Group1] DEFAULT (0) FOR [byGroup],
	CONSTRAINT [DF_MONSTER_Group2] DEFAULT (0) FOR [byActType],
	CONSTRAINT [DF_MONSTER_Type] DEFAULT (0) FOR [byType],
	CONSTRAINT [DF_MONSTER_Family] DEFAULT (0) FOR [byFamily],
	CONSTRAINT [DF_MONSTER_Rank] DEFAULT (0) FOR [byRank],
	CONSTRAINT [DF_MONSTER_Title] DEFAULT (0) FOR [byTitle],
	CONSTRAINT [DF_K_MONSTER_bySellingGroup] DEFAULT (0) FOR [iSellingGroup],
	CONSTRAINT [DF_MONSTER_Level] DEFAULT (1) FOR [sLevel],
	CONSTRAINT [DF_MONSTER_Exp] DEFAULT (0) FOR [iExp],
	CONSTRAINT [DF_MONSTER_Loyalty] DEFAULT (0) FOR [iLoyalty],
	CONSTRAINT [DF_MONSTER_HP] DEFAULT (0) FOR [sHpPoint],
	CONSTRAINT [DF_MONSTER_MP] DEFAULT (0) FOR [sMpPoint],
	CONSTRAINT [DF_MONSTER_Atk] DEFAULT (1) FOR [sAtk],
	CONSTRAINT [DF_MONSTER_Ac] DEFAULT (1) FOR [sAc],
	CONSTRAINT [DF_MONSTER_HitRate] DEFAULT (1) FOR [sHitRate],
	CONSTRAINT [DF_MONSTER_EvadeRate] DEFAULT (1) FOR [sEvadeRate],
	CONSTRAINT [DF_MONSTER_Damage] DEFAULT (1) FOR [sDamage],
	CONSTRAINT [DF_MONSTER_Delay] DEFAULT (0) FOR [sAttackDelay],
	CONSTRAINT [DF_MONSTER_Speed] DEFAULT (0) FOR [bySpeed1],
	CONSTRAINT [DF_MONSTER_Standtime] DEFAULT (0) FOR [sStandtime],
	CONSTRAINT [DF_MONSTER_Magic1] DEFAULT (0) FOR [iMagic1],
	CONSTRAINT [DF_MONSTER_Magic2] DEFAULT (0) FOR [iMagic2],
	CONSTRAINT [DF_MONSTER_Magic3] DEFAULT (0) FOR [iMagic3],
	CONSTRAINT [DF_MONSTER_FireR] DEFAULT (0) FOR [byFireR],
	CONSTRAINT [DF_MONSTER_ColdR] DEFAULT (0) FOR [byColdR],
	CONSTRAINT [DF_MONSTER_LightR] DEFAULT (0) FOR [byLightningR],
	CONSTRAINT [DF_MONSTER_MagicR] DEFAULT (0) FOR [byMagicR],
	CONSTRAINT [DF_MONSTER_DiseaseR] DEFAULT (0) FOR [byDiseaseR],
	CONSTRAINT [DF_MONSTER_PoisonR] DEFAULT (0) FOR [byPoisonR],
	CONSTRAINT [DF_K_MONSTER_sBulk] DEFAULT (0) FOR [sBulk],
	CONSTRAINT [DF_MONSTER_Sight] DEFAULT (0) FOR [byAttackRange],
	CONSTRAINT [DF_MONSTER_Range] DEFAULT (0) FOR [bySearchRange],
	CONSTRAINT [DF_K_MONSTER_byTracingRange] DEFAULT (0) FOR [byTracingRange],
	CONSTRAINT [DF_MONSTER_Gold] DEFAULT (0) FOR [iMoney],
	CONSTRAINT [DF_MONSTER_Item] DEFAULT (0) FOR [sItem],
	CONSTRAINT [DF_MONSTER_Memory] DEFAULT (0) FOR [byDirectAttack],
	CONSTRAINT [DF_K_MONSTER_byMagicAttack] DEFAULT (0) FOR [byMagicAttack]
GO

ALTER TABLE [dbo].[K_NPC] WITH NOCHECK ADD 
	CONSTRAINT [DF_KNIGHT_NPC_byWeapon] DEFAULT (0) FOR [iWeapon2],
	CONSTRAINT [DF_KNIGHT_NPC_byGroup] DEFAULT (0) FOR [byGroup],
	CONSTRAINT [DF_KNIGHT_NPC_byActType] DEFAULT (0) FOR [byActType],
	CONSTRAINT [DF_KNIGHT_NPC_byType] DEFAULT (0) FOR [byType],
	CONSTRAINT [DF_KNIGHT_NPC_byFamily] DEFAULT (0) FOR [byFamily],
	CONSTRAINT [DF_KNIGHT_NPC_byRank] DEFAULT (0) FOR [byRank],
	CONSTRAINT [DF_KNIGHT_NPC_byTitle] DEFAULT (0) FOR [byTitle],
	CONSTRAINT [DF_K_NPC_bySellingGroup] DEFAULT (0) FOR [iSellingGroup],
	CONSTRAINT [DF_KNIGHT_NPC_byLevel] DEFAULT (1) FOR [sLevel],
	CONSTRAINT [DF_KNIGHT_NPC_iExp] DEFAULT (0) FOR [iExp],
	CONSTRAINT [DF_KNIGHT_NPC_iLoyalty] DEFAULT (0) FOR [iLoyalty],
	CONSTRAINT [DF_KNIGHT_NPC_sHpPoint] DEFAULT (0) FOR [sHpPoint],
	CONSTRAINT [DF_KNIGHT_NPC_sMpPoint] DEFAULT (0) FOR [sMpPoint],
	CONSTRAINT [DF_KNIGHT_NPC_sAtk] DEFAULT (1) FOR [sAtk],
	CONSTRAINT [DF_KNIGHT_NPC_sAc] DEFAULT (1) FOR [sAc],
	CONSTRAINT [DF_KNIGHT_NPC_sHitRate] DEFAULT (1) FOR [sHitRate],
	CONSTRAINT [DF_KNIGHT_NPC_sEvadeRate] DEFAULT (1) FOR [sEvadeRate],
	CONSTRAINT [DF_KNIGHT_NPC_sDamage] DEFAULT (1) FOR [sDamage],
	CONSTRAINT [DF_KNIGHT_NPC_sAttackDelay] DEFAULT (0) FOR [sAttackDelay],
	CONSTRAINT [DF_KNIGHT_NPC_bySpeed1] DEFAULT (0) FOR [bySpeed1],
	CONSTRAINT [DF_KNIGHT_NPC_sStandtime] DEFAULT (0) FOR [sStandtime],
	CONSTRAINT [DF_KNIGHT_NPC_sMagic1] DEFAULT (0) FOR [iMagic1],
	CONSTRAINT [DF_KNIGHT_NPC_sMagic2] DEFAULT (0) FOR [iMagic2],
	CONSTRAINT [DF_KNIGHT_NPC_sMagic3] DEFAULT (0) FOR [iMagic3],
	CONSTRAINT [DF_KNIGHT_NPC_byFireR] DEFAULT (0) FOR [byFireR],
	CONSTRAINT [DF_KNIGHT_NPC_byColdR] DEFAULT (0) FOR [byColdR],
	CONSTRAINT [DF_KNIGHT_NPC_byLightR] DEFAULT (0) FOR [byLightningR],
	CONSTRAINT [DF_KNIGHT_NPC_byMagicR] DEFAULT (0) FOR [byMagicR],
	CONSTRAINT [DF_KNIGHT_NPC_byDiseaseR] DEFAULT (0) FOR [byDiseaseR],
	CONSTRAINT [DF_KNIGHT_NPC_byPoisonR] DEFAULT (0) FOR [byPoisonR],
	CONSTRAINT [DF_K_NPC_sBulk] DEFAULT (0) FOR [sBulk],
	CONSTRAINT [DF_KNIGHT_NPC_sAttackRange] DEFAULT (0) FOR [byAttackRange],
	CONSTRAINT [DF_KNIGHT_NPC_sSearchRange] DEFAULT (0) FOR [bySearchRange],
	CONSTRAINT [DF_K_NPC_byTracingRange] DEFAULT (0) FOR [byTracingRange],
	CONSTRAINT [DF_KNIGHT_NPC_sMoney] DEFAULT (0) FOR [iMoney],
	CONSTRAINT [DF_KNIGHT_NPC_sItem] DEFAULT (0) FOR [sItem],
	CONSTRAINT [DF_KNIGHT_NPC_byMemory] DEFAULT (0) FOR [byDirectAttack],
	CONSTRAINT [DF_K_NPC_byMagicAttack] DEFAULT (0) FOR [byMagicAttack]
GO

ALTER TABLE [dbo].[MAGIC] WITH NOCHECK ADD 
	CONSTRAINT [DF_MAGIC_Num] DEFAULT (1) FOR [MagicNum],
	CONSTRAINT [DF_MAGIC_Moral] DEFAULT (0) FOR [KrName],
	CONSTRAINT [DF_MAGIC_Class] DEFAULT (0) FOR [Description],
	CONSTRAINT [DF_MAGIC_Level] DEFAULT (0) FOR [BeforeAction],
	CONSTRAINT [DF_MAGIC_Effect] DEFAULT (0) FOR [TargetAction],
	CONSTRAINT [DF_MAGIC_Mp] DEFAULT (0) FOR [SelfEffect],
	CONSTRAINT [DF_MAGIC_Hp] DEFAULT (0) FOR [FlyingEffect],
	CONSTRAINT [DF_MAGIC_St] DEFAULT (0) FOR [TargetEffect],
	CONSTRAINT [DF_MAGIC_NItem] DEFAULT (0) FOR [Moral],
	CONSTRAINT [DF_MAGIC_Contime] DEFAULT (0) FOR [SkillLevel],
	CONSTRAINT [DF_MAGIC_Repeat] DEFAULT (0) FOR [Skill],
	CONSTRAINT [DF_MAGIC_Expect] DEFAULT (1) FOR [Msp],
	CONSTRAINT [DF_MAGIC_Target] DEFAULT (0) FOR [CastTime],
	CONSTRAINT [DF_MAGIC_Range] DEFAULT (0) FOR [SuccessRate],
	CONSTRAINT [DF_MAGIC_Type] DEFAULT (0) FOR [Type1]
GO

ALTER TABLE [dbo].[MAKE_ITEM] WITH NOCHECK ADD 
	CONSTRAINT [DF_MAKE_ITEM_byItemLevel] DEFAULT (0) FOR [byItemLevel]
GO

ALTER TABLE [dbo].[MAKE_ITEM_LARECODE] WITH NOCHECK ADD 
	CONSTRAINT [DF_MAKE_ITEM_LARECODE_sLareItem] DEFAULT (0) FOR [sLareItem],
	CONSTRAINT [DF_MAKE_ITEM_LARECODE_sMagicItem] DEFAULT (0) FOR [sMagicItem],
	CONSTRAINT [DF_MAKE_ITEM_LARECODE_sGereralItem] DEFAULT (0) FOR [sGereralItem]
GO

ALTER TABLE [dbo].[USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [DF_USERDATA_Nation] DEFAULT (0) FOR [Nation],
	CONSTRAINT [DF_USERDATA_Race] DEFAULT (1) FOR [Race],
	CONSTRAINT [DF_USERDATA_Class] DEFAULT (0) FOR [Class],
	CONSTRAINT [DF_USERDATA_Gender] DEFAULT (0) FOR [HairColor],
	CONSTRAINT [DF_USERDATA_Rank] DEFAULT (0) FOR [Rank],
	CONSTRAINT [DF_USERDATA_Title] DEFAULT (0) FOR [Title],
	CONSTRAINT [DF_USERDATA_Level] DEFAULT (1) FOR [Level],
	CONSTRAINT [DF_USERDATA_Exp] DEFAULT (0) FOR [Exp],
	CONSTRAINT [DF_USERDATA_Loyalty] DEFAULT (0) FOR [Loyalty],
	CONSTRAINT [DF_USERDATA_Religion] DEFAULT (0) FOR [Face],
	CONSTRAINT [DF_USERDATA_City] DEFAULT (0) FOR [City],
	CONSTRAINT [DF_USERDATA_Knights] DEFAULT (0) FOR [Knights],
	CONSTRAINT [DF_USERDATA_Fame] DEFAULT (0) FOR [Fame],
	CONSTRAINT [DF_USERDATA_Hp] DEFAULT (100) FOR [Hp],
	CONSTRAINT [DF_USERDATA_Mp] DEFAULT (100) FOR [Mp],
	CONSTRAINT [DF_USERDATA_Sp] DEFAULT (100) FOR [Sp],
	CONSTRAINT [DF_USERDATA_Str] DEFAULT (0) FOR [Strong],
	CONSTRAINT [DF_USERDATA_Sta] DEFAULT (0) FOR [Sta],
	CONSTRAINT [DF_USERDATA_Dex] DEFAULT (0) FOR [Dex],
	CONSTRAINT [DF_USERDATA_Intel] DEFAULT (0) FOR [Intel],
	CONSTRAINT [DF_USERDATA_Cha] DEFAULT (0) FOR [Cha],
	CONSTRAINT [DF_USERDATA_Authority] DEFAULT (1) FOR [Authority],
	CONSTRAINT [DF_USERDATA_Points] DEFAULT (0) FOR [Points],
	CONSTRAINT [DF_USERDATA_Gold] DEFAULT (0) FOR [Gold],
	CONSTRAINT [DF_USERDATA_Zone] DEFAULT (1) FOR [Zone],
	CONSTRAINT [DF_USERDATA_PX] DEFAULT (268100) FOR [PX],
	CONSTRAINT [DF_USERDATA_PZ] DEFAULT (131000) FOR [PZ],
	CONSTRAINT [DF_USERDATA_PY] DEFAULT (0) FOR [PY],
	CONSTRAINT [DF_USERDATA_dwTime] DEFAULT (0) FOR [dwTime],
	CONSTRAINT [DF_USERDATA_strSkill] DEFAULT (0x00) FOR [strSkill]
GO

ALTER TABLE [dbo].[WAREHOUSE] WITH NOCHECK ADD 
	CONSTRAINT [DF_WAREHOUSE_nMoney] DEFAULT (0) FOR [nMoney],
	CONSTRAINT [DF_WAREHOUSE_dwTime] DEFAULT (0) FOR [dwTime]
GO

ALTER TABLE [dbo].[ZONE_INFO] WITH NOCHECK ADD 
	CONSTRAINT [DF_ZONE_INFO2_ServerNo] DEFAULT (1) FOR [ServerNo],
	CONSTRAINT [DF_ZONE_INFO2_InitX] DEFAULT (1000) FOR [InitX],
	CONSTRAINT [DF_ZONE_INFO2_InitZ] DEFAULT (1000) FOR [InitZ],
	CONSTRAINT [DF_ZONE_INFO2_InitY] DEFAULT (0) FOR [InitY],
	CONSTRAINT [DF_ZONE_INFO2_Type] DEFAULT (1) FOR [Type]
GO

 CREATE  INDEX [IX_rating_rank] ON [dbo].[KNIGHTS_RATING]([nRank]) ON [PRIMARY]
GO

 CREATE  INDEX [IX_rating_name] ON [dbo].[KNIGHTS_RATING]([strName]) ON [PRIMARY]
GO

 CREATE  INDEX [PK_Knights] ON [dbo].[USERDATA]([Knights]) ON [PRIMARY]
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE CREATE_KNIGHTS2

@nRet		smallint OUTPUT, 
@index 		smallint OUTPUT,
@nation		tinyint,
@community	tinyint,
@strName 	char(21), 
@strChief	char(21)

AS

DECLARE @Row tinyint, @knightsindex smallint, @knightsname char(21)
	SET @Row = 0	SET @knightsindex = 0  SET @knightsname = ''

	SELECT @Row = COUNT(*) FROM KNIGHTS WHERE IDNum = @index
	
	IF @Row > 0 or @index = 0
	BEGIN
		SET @nRet =  1
		RETURN
	END
	
BEGIN TRAN	

	INSERT INTO KNIGHTS ( IDNum, Nation, Flag, IDName, Chief  ) 
	VALUES	 (@index, @nation, @community, @strName, @strChief )
	
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  2
		RETURN
	END

	UPDATE USERDATA SET Knights = @index, Fame = 1 WHERE strUserId = @strChief	-- 1 == Chief Authority
	
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  3
		RETURN
	END

COMMIT TRAN
SET @nRet =  0
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- Create sungyong

CREATE PROCEDURE EDITER_KNIGHTS
AS

SET NOCOUNT ON
DECLARE @KnightsIndex smallint
DECLARE job1 CURSOR FOR

SELECT IDNum FROM KNIGHTS

OPEN job1
FETCH NEXT FROM job1
INTO @KnightsIndex
WHILE @@fetch_status = 0 
BEGIN
	DECLARE @LEVEL INT
	DECLARE @ROW INT
	SET @LEVEL = 0

	SELECT @ROW = COUNT([strUserId]) FROM [USERDATA] WHERE Knights = @KnightsIndex
	IF @ROW <> 0
		BEGIN
			UPDATE KNIGHTS SET [Members]=@Row WHERE IDNum = @KnightsIndex
		END
	ELSE
		BEGIN
			DELETE FROM KNIGHTS WHERE IDNum = @knightsindex
		ENd
	
	FETCH NEXT FROM job1
	INTO @KnightsIndex
END
CLOSE job1
DEALLOCATE job1
SET NOCOUNT OFF
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- create by sungyong 2002.09.16

CREATE PROCEDURE EXEC_KNIGHTS_USER
AS

SET NOCOUNT ON
DECLARE @strUserID char(21)
DECLARE @KnightsIndex smallint
DECLARE job1 CURSOR FOR

SELECT strUserID, Knights FROM USERDATA

OPEN job1
FETCH NEXT FROM job1
INTO @strUserID, @KnightsIndex
WHILE @@fetch_status = 0 
BEGIN
	IF @KnightsIndex <> 0
		BEGIN
			INSERT INTO KNIGHTS_USER ( sIDNum, strUserID ) VALUES (@KnightsIndex, @strUserID )
		END
	
	FETCH NEXT FROM job1
	INTO @strUserID, @KnightsIndex
END
CLOSE job1
DEALLOCATE job1
SET NOCOUNT OFF
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE EXEC_NPCPOS_SORT
AS

BEGIN TRAN

	DROP TABLE K_NPCPOS_SORT
	
	SELECT *
	INTO K_NPCPOS_SORT
	FROM K_NPCPOS
            ORDER BY ZoneID	
	
	IF @@ERROR <> 0 
		ROLLBACK TRAN
		

	DROP TABLE K_NPCPOS
	IF @@ERROR <> 0 
		ROLLBACK TRAN

	SELECT *
	INTO K_NPCPOS
	FROM K_NPCPOS_SORT
	
	IF @@ERROR <> 0
		ROLLBACK TRAN

COMMIT TRAN

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- Created By Sungyong 2002. 10. 14

CREATE PROCEDURE KNIGHTS_RATING_UPDATE AS

if exists (select * from sysobjects where id = object_id(N'[dbo].[rating_temp]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[rating_temp]
CREATE TABLE [dbo].[rating_temp] (
	[nRank] [int] IDENTITY (1, 1) NOT NULL ,
	[shIndex] [smallint] NULL,
	[strName] [char] (21) NULL ,
	[nPoints] [int] NULL ,
) ON [PRIMARY]


INSERT INTO rating_temp SELECT IDNum, IDName, Points FROM KNIGHTS ORDER BY Points DESC

CREATE  INDEX [IX_rating_rank] ON [dbo].[rating_temp]([nRank]) ON [PRIMARY]
CREATE  INDEX [IX_rating_name] ON [dbo].[rating_temp]([strName]) ON [PRIMARY]

if exists (select * from sysobjects where id = object_id(N'[dbo].[KNIGHTS_RATING]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[KNIGHTS_RATING]
EXEC sp_rename 'rating_temp','KNIGHTS_RATING'
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO



-- Scripted by Samma
-- 2002.01.18

CREATE PROCEDURE LOAD_CHAR_INFO
@CharId	char(21),
@nRet		smallint OUTPUT
AS

SELECT @nRet = COUNT(strUserId) FROM USERDATA WHERE strUserId = @CharId
IF @nRet = 0
	RETURN

SELECT Race, Class, HairColor, [Level], Face, strItem FROM USERDATA WHERE strUserID = @CharId

SET @nRet = 1
RETURN
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- Scripted by Sungyong
-- 2002.09.26

CREATE PROCEDURE LOAD_KNIGHTS_MEMBERS
@knightsindex	smallint
AS

SELECT strUserId, Fame, [Level], Class FROM USERDATA WHERE Knights = @knightsindex

--SET @nRet = 1
--RETURN
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

-- Created by Samma
-- 2002.01.18

CREATE PROCEDURE ACCOUNT_LOGIN
@AccountID	varchar(21),
@Password	varchar(13),
@nRet		smallint	OUTPUT

AS


DECLARE @Nation tinyint, @CharNum smallint
SET @Nation = 0
SET @CharNum = 0

DECLARE @pwd varchar(13)

SET @pwd = null

SELECT @pwd = strPasswd FROM  [218.50.5.40].[knight_Account].[dbo].[TB_USER] WHERE strAccountID = @AccountID
IF @pwd IS null
BEGIN
	SET @nRet = 0
	RETURN
END

ELSE IF @pwd <> @Password
BEGIN
	SET @nRet = 0
	RETURN
END

SELECT @Nation = bNation, @CharNum = bCharNum FROM ACCOUNT_CHAR WHERE strAccountID = @AccountID
IF @@ROWCOUNT = 0
BEGIN
	SET @nRet = 1
	RETURN
END
IF @CharNum = 0
BEGIN
	SET @nRet = 1
	RETURN
END
ELSE 
BEGIN
	SET @nRet = @Nation+1
	RETURN
END
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_FRI] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_FRI_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_FRI_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_FRI_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_FRI_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_FRI_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_FRI_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_MON] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_MON_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_MON_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_MON_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_MON_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_MON_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_MON_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_SAT] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_SAT_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_SAT_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_SAT_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_SAT_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_SAT_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_SAT_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_SUN] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_SUN_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_SUN_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_SUN_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_SUN_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_SUN_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_SUN_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_THU] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_THU_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_THU_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_THU_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_THU_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_THU_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_THU_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_TUE] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_TUE_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_TUE_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_TUE_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_TUE_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_TUE_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_TUE_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- Scripted by Kibum Yook
-- 2002.08.21
-- Created by Yookozuna because online game players are dumb :(

CREATE PROCEDURE BACKUP_USERDATA AS

DECLARE	@Rank tinyint, @Title tinyint, @Level tinyint
DECLARE	@Exp int, @Loyalty int
DECLARE	@Knights smallint, @Fame tinyint, @Strong tinyint, @Sta tinyint, @Dex tinyint
DECLARE	@Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @Zone tinyint
DECLARE	@strSkill varchar(20), @strItem varchar(400)

BEGIN TRAN	

	SELECT @Rank = Rank, @Title = Title, @Level = [Level], @Exp = [Exp], @Loyalty = Loyalty, @Knights = Knights, @Fame = Fame, 
		 @Strong = Strong, @Sta = Sta, @Dex = Dex, @Intel = Intel, @Cha = Cha, 
		 @Points = Points, @Gold = Gold,@strSkill = strSkill, @strItem = strItem
	FROM BK_TUE_USERDATA /*날짜 확인*/ WHERE strUserId = '나여요'     -- 이 부분은 이름만 수정하면 되요 ^^;

	INSERT INTO USERDATA (strUserID, Nation, Race, Class, HairColor, Rank, Title, [Level], [Exp], Loyalty, Face, City,
				      Knights, Fame, Hp, Mp, Sp, Strong, Sta, Dex, Intel, Cha, Authority, Points, Gold, [Zone], Bind, PX, PZ, PY, strSkill, strItem )

	VALUES (/*@Char*/'나여요' , /*@Nation*/ 2, /*@Race*/ 1, /*@Class*/ 201, /*@HairColor*/ 0, @Rank, @Title, @Level, @Exp,
 		   @Loyalty, /* @Face*/0, /*@City*/0, @Knights, @Fame, /*@Hp*/100, /*@Mp*/100, /*@Sp*/100, @Strong, @Sta, @Dex, @Intel, @Cha,
 		  /*@Authority*/1, @Points, @Gold, /*@Zone*/2, /*@Bind*/ -1, /*@PX*/50000, /*@PZ*/50000, /*@PY*/0, @strSkill, @strItem ) 
		
		 -- 여긴 주석 된 부분 만 입력해주면 되요 ^^
	
COMMIT TRAN
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- Scripted by Kibum Yook
-- 2002.10.08
-- Created by Yookozuna because online game players are dumb :(

CREATE PROCEDURE BACKUP_WAREHOUSE AS

DECLARE	@WarehouseData varchar(1600), @strAccountID char(21), @nMoney int

BEGIN TRAN	

	SELECT @WarehouseData = WarehouseData, @nMoney = nMoney
	FROM BK_MON_WAREHOUSE WHERE strAccountID = '97414061'     -- 이 부분은 이름만 수정하면 되요, 날짜두요 ^^;

	INSERT INTO WAREHOUSE (strAccountID, nMoney, WarehouseData )

	VALUES ('97414061', @nMoney, @WarehouseData) 
			
COMMIT TRAN
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


--create by samma 2002.07.25

CREATE PROCEDURE [BACKUP_WED] AS

declare @strUserID char(21), @Rank tinyint, @Title tinyint, @Level tinyint, @Exp int, @Loyalty int, @Knights smallint, @Fame tinyint
declare @Str tinyint, @Sta tinyint, @Dex tinyint, @Intel tinyint, @Cha tinyint, @Points tinyint, @Gold int, @strSkill varchar(10), @strItem varchar(400)

SET @strUserID = null SET @Rank = 0 SET @Title = 0 SET @Level = 0 SET @Exp = 0 SET @Loyalty = 0 SET @Knights = 0 SET @Fame = 0
SET @Str = 0 SET @Sta = 0 SET @Dex = 0 SET @Intel = 0 SET @Cha = 0 SET @Points = 0 SET @Gold = 0 SET @strSkill = null SET @strItem = null

declare @row tinyint
SET @row = 0

DECLARE Backup_Cursor CURSOR
	FOR
	SELECT strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem FROM USERDATA

	OPEN Backup_Cursor

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_WED_USERDATA where strUserID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_WED_USERDATA (strUserID, Rank, Title, [Level], [Exp], Loyalty, Knights, Fame, Strong, Sta, Dex, Intel, Cha, Points, Gold, strSkill, strItem)
		VALUES (@strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_WED_USERDATA 
		SET Rank = @Rank, Title = @Title, [Level] = @Level, [Exp] = @Exp, Loyalty = @Loyalty, Knights = @Knights, Fame = @Fame, Strong = @Str, Sta = @Sta, Dex = @Dex, Intel = @Intel, Cha = @Cha, Points = @Points, Gold = @Gold, strSkill = @strSkill, strItem = @strItem
		WHERE strUserID = @strUserID
	END
	
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Rank, @Title, @Level, @Exp, @Loyalty, @Knights, @Fame, @Str, @Sta, @Dex, @Intel, @Cha, @Points, @Gold, @strSkill, @strItem
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

declare @nMoney int, @WarehouseData varchar(1600)
SET @nMoney = 0 SET @WarehouseData = null

DECLARE Backup_Item_Cursor CURSOR
	FOR
	SELECT strAccountID, nMoney, WarehouseData FROM WAREHOUSE

	OPEN Backup_Item_Cursor

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData

WHILE @@FETCH_STATUS = 0
BEGIN
	select @row = count(*) from BK_WED_WAREHOUSE where strAccountID = @strUserID
	IF @row = 0 
	BEGIN
		INSERT INTO BK_WED_WAREHOUSE (strAccountID, nMoney, warehouseData)
		VALUES (@strUserID, @nMoney, @WarehouseData)
	END
	IF @row > 0
	BEGIN
		UPDATE BK_WED_WAREHOUSE
		SET nMoney = @nMoney, warehouseData = @WarehouseData WHERE strAccountID = @strUserID
	END

	FETCH NEXT FROM Backup_Item_Cursor INTO @strUserID, @nMoney, @WarehouseData
END

CLOSE Backup_Item_Cursor
DEALLOCATE Backup_Item_Cursor

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

CREATE PROCEDURE CHECK_KNIGHTS
AS

SET NOCOUNT ON
DECLARE @KnightsIndex smallint
DECLARE job1 CURSOR FOR

SELECT IDNum FROM KNIGHTS

OPEN job1
FETCH NEXT FROM job1
INTO @KnightsIndex
WHILE @@fetch_status = 0 
BEGIN
	DECLARE @ROW INT

	SELECT @ROW = Members FROM [KNIGHTS] WHERE [IDNum] = @KnightsIndex
	IF @ROW = 1
	BEGIN
		BEGIN TRAN
			DELETE FROM KNIGHTS WHERE IDNum = @knightsindex
			
			IF @@ERROR <> 0
				BEGIN
					ROLLBACK TRAN
				END
			ELSE
				BEGIN
					UPDATE USERDATA SET Knights = 0, Fame = 0 WHERE Knights = @knightsindex
					DELETE FROM KNIGHTS_USER WHERE [sIDNum] = @KnightsIndex
				END
		COMMIT TRAN
	END
	
	FETCH NEXT FROM job1
	INTO @KnightsIndex
END
CLOSE job1
DEALLOCATE job1
SET NOCOUNT OFF
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- modify by sungyong 2002.09.27

CREATE PROCEDURE CREATE_KNIGHTS

@nRet		smallint OUTPUT, 
@index 		smallint,
@nation		tinyint,
@community	tinyint,
@strName 	char(21), 
@strChief	char(21)

AS

DECLARE @Row tinyint, @knightsindex smallint, @knightsname char(21)
	SET @Row = 0	SET @knightsindex = 0  SET @knightsname = ''

	SELECT @Row = COUNT(*) FROM KNIGHTS WHERE IDNum = @index or  IDName = @strName
	
	IF @Row > 0 or @index = 0
	BEGIN
		SET @nRet =  3
		RETURN
	END

	--SELECT @Row = COUNT(*) FROM KNIGHTS WHERE IDName = @strName
	
	--IF @Row > 0
--	BEGIN
	--	SET @nRet =  3
		--RETURN
--	END
	
BEGIN TRAN	

	INSERT INTO KNIGHTS ( IDNum, Nation, Flag, IDName, Chief  ) 
	VALUES	 (@index, @nation, @community, @strName, @strChief )

	INSERT INTO KNIGHTS_USER ( sIDNum, strUserID  ) 
	VALUES	 (@index,  @strChief )
	
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  6
		RETURN
	END

--	UPDATE USERDATA SET Knights = @index, Fame = 1 WHERE strUserId = @strChief	-- 1 == Chief Authority
	
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  6
		RETURN
	END

COMMIT TRAN
SET @nRet =  0
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



-- scripted by samma
-- 2002.01.17

CREATE PROCEDURE CREATE_NEW_CHAR

@nRet		smallint OUTPUT, 
@AccountID 	char(21), 
@index 		tinyint,
@CharID	char(21),
@Race 		tinyint, 
@Class 	smallint, 
@Hair 		tinyint,
@Face 		tinyint, 
@Str 		tinyint, 
@Sta 		tinyint, 
@Dex 		tinyint,
@Intel 		tinyint, 
@Cha 		tinyint

AS

DECLARE @Row tinyint, @Nation tinyint, @Zone tinyint, @PosX int, @PosZ int
	SET @Row = 0	SET @Nation = 0  SET @Zone = 0  SET @PosX = 0 SET @PosZ = 0

	SELECT @Nation = bNation, @Row = bCharNum FROM ACCOUNT_CHAR WHERE strAccountID = @AccountID
	
	IF @Row >= 5	SET @nRet =  1
	
	IF @Nation = 1 AND @Race > 10	SET @nRet = 2
	ELSE IF @Nation = 2 AND @Race < 10	SET @nRet = 2
	ELSE IF @Nation <>1 AND @Nation <> 2	SET @nRet = 2

	IF @nRet > 0
		RETURN
	
	SELECT @Row = COUNT(*) FROM USERDATA WHERE strUserId = @CharID
	IF @Row > 0 
	BEGIN
		SET @nRet =  3
		RETURN
	END

	SET @Zone = @Nation
	SELECT @PosX = InitX, @PosZ = InitZ  FROM ZONE_INFO WHERE ZoneNo = @Zone

BEGIN TRAN	
	IF @index = 0
		UPDATE ACCOUNT_CHAR SET strCharID1 = @CharID, bCharNum = bCharNum + 1 WHERE strAccountID = @AccountID
	ELSE IF @index = 1
		UPDATE ACCOUNT_CHAR SET strCharID2 = @CharID, bCharNum = bCharNum + 1 WHERE strAccountID = @AccountID
	ELSE IF @index = 2
		UPDATE ACCOUNT_CHAR SET strCharID3 = @CharID, bCharNum = bCharNum + 1 WHERE strAccountID = @AccountID
	ELSE IF @index = 3
		UPDATE ACCOUNT_CHAR SET strCharID4 = @CharID, bCharNum = bCharNum + 1 WHERE strAccountID = @AccountID
	ELSE IF @index = 4
		UPDATE ACCOUNT_CHAR SET strCharID5 = @CharID, bCharNum = bCharNum + 1 WHERE strAccountID = @AccountID
	

	INSERT INTO USERDATA (strUserId, Nation, Race, Class, HairColor, Face, Strong, Sta, Dex, Intel, Cha, Zone, PX, PZ ) 
	VALUES	 (@CharID, @Nation, @Race, @Class, @Hair, @Face, @Str, @Sta, @Dex, @Intel, @Cha, @Zone, @PosX, @PosZ )
	
	
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  4
		RETURN
	END
	
COMMIT TRAN
SET @nRet =  0
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

-- Scripted by Samma
-- 2002.01.18
-- 2002.08.19 Modified by samma because check social number
-- 2002.10.11 Modified by Sungyong  because knights, knights_user table modify

CREATE PROCEDURE DELETE_CHAR
@AccountID	char(21),
@index		tinyint,
@CharID	char(21),
@SocNo	char(15),
@nRet		smallint OUTPUT
AS

DECLARE @bCharNum	tinyint, @charid1 char(21), @charid2 char(21), @charid3 char(21), @charid4 char(21), @charid5 char(21), @strSocNo char(15)
DECLARE @knightsindex smallint
	SET @bCharNum = 0
	SET @knightsindex = 0

	SELECT @strSocNo = strSocNo FROM [218.50.5.40].[Knight_Account].[dbo].[TB_USER] WHERE strAccountID = @AccountID
	IF @SocNo <> @strSocNo
	BEGIN
	SET @nRet = 0
	RETURN
	END

DECLARE	@Nation tinyint, @Race	tinyint, @Class	smallint, @HairColor tinyint, @Rank tinyint, @Title	tinyint, @Level tinyint, @Exp int, @Loyalty int
DECLARE	@Face	tinyint, @City tinyint, @Knights	smallint, @Fame tinyint,	@Hp smallint, @Mp smallint,  @Sp smallint, @Strong tinyint, @Sta tinyint, @Dex tinyint
DECLARE	@Intel tinyint, @Cha tinyint, @Authority	tinyint, @Points	tinyint,	@Gold int, @Zone tinyint, @Bind	smallint, @PX int, @PZ int, @PY int, @strSkill varchar(20),	@strItem varchar(400)
DECLARE          @Members smallint

BEGIN TRAN

	IF @index = 0
		UPDATE ACCOUNT_CHAR SET strCHARID1 = NULL, bCharNum = bCharNum - 1 WHERE strAccountID = @AccountID
	IF @index = 1
		UPDATE ACCOUNT_CHAR SET strCHARID2 = NULL, bCharNum = bCharNum - 1 WHERE strAccountID = @AccountID
	IF @index = 2
		UPDATE ACCOUNT_CHAR SET strCHARID3 = NULL, bCharNum = bCharNum - 1 WHERE strAccountID = @AccountID
	IF @index = 3
		UPDATE ACCOUNT_CHAR SET strCHARID4 = NULL, bCharNum = bCharNum - 1 WHERE strAccountID = @AccountID
	IF @index = 4
		UPDATE ACCOUNT_CHAR SET strCHARID5 = NULL, bCharNum = bCharNum - 1 WHERE strAccountID = @AccountID

	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN
		SET @nRet = 0
		RETURN 
	END

	SELECT @charid1 = strCHARID1, @charid2 = strCHARID2, @charid3 = strCHARID3, @charid4 = strCHARID4, @charid5 = strCHARID5 FROM ACCOUNT_CHAR WHERE strAccountID = @AccountID
	IF @charid1 = NULL and @charid2 = NULL and @charid3 = NULL and @charid4 = NULL and @charid5 = NULL 	 -- 캐릭터가 하나도 없으면 해당 레코드를 지운다.. -> 국가선택 다시 할수 있다
	BEGIN
		DELETE FROM ACCOUNT_CHAR WHERE strAccountID = @AccountID
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN
			SET @nRet = 0
			RETURN
		END
	END

	SELECT @Nation = Nation, @Race = Race, @Class = Class, @HairColor = HairColor, @Rank = Rank, @Title = Title, @Level = [Level], @Exp = [Exp], @Loyalty = Loyalty, @Face = Face, @City = City, @Knights = Knights, @Fame = Fame, 
		 @Hp = Hp, @Mp = Mp, @Sp = Sp, @Strong = Strong, @Sta = Sta, @Dex = Dex, @Intel = Intel, @Cha = Cha, @Authority = Authority, @Points = Points, @Gold = Gold, @Zone = [Zone], @Bind = Bind, @PX = PX, @PZ = PZ, @PY = PY, @strSkill = strSkill, @strItem = strItem
	FROM	USERDATA WHERE strUserId = @CharID

	INSERT INTO DELETED_USERDATA (strAccountID, strUserID, Nation, Race, Class, HairColor, Rank, Title, [Level], [Exp], Loyalty, Face, City, Knights, Fame, Hp, Mp, Sp, Strong, Sta, Dex, Intel, Cha, Authority, Points, Gold, [Zone], Bind, PX, PZ, PY, strSkill, strItem )
	VALUES ( @AccountID, @CharID, @Nation, @Race, @Class, @HairColor, @Rank, @Title, @Level, @Exp, @Loyalty, @Face, @City, @Knights, @Fame, @Hp, @Mp, @Sp, @Strong, @Sta, @Dex, @Intel, @Cha, @Authority, @Points, @Gold, @Zone, @Bind, @PX, @PZ, @PY, @strSkill, @strItem )

	DELETE FROM USERDATA WHERE strUserId = @CharID
	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN
		SET @nRet = 0
		RETURN
	END

	DELETE FROM KNIGHTS_USER WHERE strUserId = @CharID
	SELECT @Members=Members FROM KNIGHTS WHERE IDNum = @Knights
	IF @Members <= 1
		UPDATE KNIGHTS SET Members = 1 WHERE IDNum = @Knights
	ELSE 
		UPDATE KNIGHTS SET Members = Members - 1 WHERE IDNum = @Knights

COMMIT TRAN
SET @nRet = 1
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE DELETE_KNIGHTS
@nRet			smallint OUTPUT,
@knightsindex		smallint
AS

DECLARE @Row	tinyint, @Knights smallint, @Fame tinyint, @UserID char(21)
		SET @Row = 0 SET @Knights = 0 SET @Fame = 0 SET @UserID = ''


-- sungyong modify
SELECT @Row = COUNT(*) FROM KNIGHTS WHERE IDNum = @knightsindex
	IF @Row = 0
	BEGIN
		SET @nRet = 7
		RETURN
	END

BEGIN TRAN

	DELETE FROM KNIGHTS WHERE IDNum = @knightsindex
	DELETE FROM KNIGHTS_USER WHERE sIDNum = @knightsindex

	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN
		SET @nRet = 7
		RETURN 
	END

	-- UPDATE USERDATA SET Knights = 0, Fame = 0 WHERE Knights = @knightsindex

COMMIT TRAN
SET @nRet = 0
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- create by sungyong 2002.10.07

CREATE PROCEDURE EXEC_SEARCH_USER_ITEM
AS

SET NOCOUNT ON
DECLARE @strUserID char(21)
DECLARE @strItem char(400)
DECLARE job1 CURSOR FOR

SELECT strUserID, strItem FROM USERDATA

OPEN job1
FETCH NEXT FROM job1
INTO @strUserID, @strItem
WHILE @@fetch_status = 0 
BEGIN
	DECLARE @strAccountID char(21)
	DECLARE @strWareHouse char(1600)

	SELECT @strAccountID=strAccountID FROM ACCOUNT_CHAR WHERE @struserid=strCharID1 or @struserid=strCharID2 or @struserid=strCharID3
	SELECT @strWareHouse=WarehouseData FROM WAREHOUSE WHERE strAccountID=@strAccountID

	INSERT INTO SEARCH_USERDATA (strUserId, strAccountID, strItem, strWarehouseData ) 
	VALUES	 (@strUserID, @strAccountID, @strItem, @strWareHouse )

	
	FETCH NEXT FROM job1
	INTO @strUserID, @strItem
END
CLOSE job1
DEALLOCATE job1
SET NOCOUNT OFF
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

-- Scripted by Yookozuna
-- 2002.08.27
-- This program was made to eliminate level-up cheaters

CREATE PROCEDURE LEVEL_CHEAT AS

-----------------------------------------------------------------------------------------------------------------------
-- 변수들을 선언하는 부분

DECLARE @strUserID char(21), @Level tinyint, @row tinyint, @LevelYesterday tinyint        
SET @strUserID = null
SET @Level = 0
SET @row = 0
SET @LevelYesterday = 0

-----------------------------------------------------------------------------------------------------------------------
-- 이 부분은 오늘 날짜 20레벨 이상 유저들을 임시 데이타에 넣는다.

DECLARE Backup_Cursor CURSOR FOR                                       
	SELECT strUserID, [Level]
	FROM USERDATA WHERE [Level] >= 20

	OPEN Backup_Cursor
	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Level

WHILE @@FETCH_STATUS = 0
BEGIN
	SELECT @row = count(*) 
	FROM LEVEL_CHEAT_TEMP
	WHERE strUserID = @strUserID

	IF @row = 0 
	BEGIN
		INSERT INTO LEVEL_CHEAT_TEMP (strUserID, [Level], [LevelYesterday])
		VALUES (@strUserID, @Level, 0)
	END

	IF @row > 0
	BEGIN
		UPDATE LEVEL_CHEAT_TEMP
		SET [Level] = @Level WHERE strUserID = @strUserID
	END

	FETCH NEXT FROM Backup_Cursor INTO @strUserID, @Level
END

CLOSE Backup_Cursor
DEALLOCATE Backup_Cursor

--------------------------------------------------------------------------------------------------------------------------
-- 이 부분은 그 전의 날의 레벨들을 가져온다.

DECLARE Backup_Yesterday_Cursor CURSOR FOR                       

	SELECT strUserID, [Level]
	FROM BK_TUE_USERDATA --WHERE [Level] >= 20

	OPEN Backup_Yesterday_Cursor
	FETCH NEXT FROM Backup_Yesterday_Cursor INTO @strUserID, @Level

WHILE @@FETCH_STATUS = 0
BEGIN
	SELECT @row = count(*)
	FROM LEVEL_CHEAT_TEMP
	WHERE strUserID = @strUserID
/*
	IF @row = 0 
	BEGIN
		INSERT INTO LEVEL_CHEAT (strUserID, [LevelYesterday])
		VALUES (@strUserID, @Level)
	END
*/
	IF @row > 0
	BEGIN
		UPDATE LEVEL_CHEAT_TEMP
		SET [LevelYesterday] = @Level WHERE strUserID = @strUserID
	END

	FETCH NEXT FROM Backup_Yesterday_Cursor INTO @strUserID, @Level
END	

CLOSE Backup_Yesterday_Cursor
DEALLOCATE Backup_Yesterday_Cursor

-------------------------------------------------------------------------------------------------------------------------
-- 이 부분은 두 날 사이의 랩업을 비교해서 광랩한 유저들만 추출한다.

DECLARE Backup_Compare_Cursor CURSOR FOR

	SELECT strUserID, [Level], [LevelYesterday]
	FROM LEVEL_CHEAT_TEMP WHERE ([Level] - [LevelYesterday]) >= 5

	OPEN Backup_Compare_Cursor
	FETCH NEXT FROM Backup_Compare_Cursor INTO @strUserID, @Level, @LevelYesterday

WHILE @@FETCH_STATUS = 0
BEGIN
	SELECT @row = count(*)
	FROM LEVEL_CHEAT
	WHERE strUserID = @strUserID

	IF @row = 0
	BEGIN
		INSERT INTO LEVEL_CHEAT (strUserID, [Level], [LevelYesterday], [LevelDifference])
		VALUES (@strUserID, @Level, @LevelYesterday, @Level - @LevelYesterday)
	END

	FETCH NEXT FROM Backup_Compare_Cursor INTO  @strUserID, @Level, @LevelYesterday
END

CLOSE Backup_Compare_Cursor
DEALLOCATE Backup_Compare_Cursor
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO



-- Scripted by Samma
-- 2002.01.18

CREATE PROCEDURE LOAD_ACCOUNT_CHARID
@Accountid 	char(21)
AS

SELECT strCharID1, strCharID2, strCharID3, strCharID4, strCharID5  FROM ACCOUNT_CHAR WHERE strAccountID = @Accountid

RETURN @@ROWCOUNT
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE PROCEDURE LOAD_USER_DATA
@id	char(21)
AS

DECLARE @nRet smallint
SET @nRet = 0

SELECT @nRet = COUNT(strUserId) FROM USERDATA WHERE strUserId = @id
IF @nRet = 0
	RETURN 0

SELECT Nation, Race, Class, HairColor, Rank, Title, [Level], [Exp], Loyalty, Face, City, Knights, Fame, 
	 Hp, Mp, Sp, Strong, Sta, Dex, Intel, Cha, Authority, Points, Gold, [Zone], Bind, PX, PZ, PY, dwTime, strSkill, strItem
FROM	USERDATA WHERE strUserId = @id

RETURN 1
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO



-- Created by Samma
-- 2002.01.18

CREATE PROCEDURE NATION_SELECT
@nRet		smallint	OUTPUT,
@AccountID 	char(21),
@Nation	tinyint
AS

DECLARE @Row tinyint
	SET @Row = 0

--	SELECT @Row = COUNT(*) FROM  ACCOUNT_CHAR  WHERE strAccountID = @AccountID
--	IF @Row > 0	
--	BEGIN
--		SET @nRet = -1
--		RETURN
--	END

BEGIN TRAN	

	SELECT @Row = COUNT(*) FROM  ACCOUNT_CHAR  WHERE strAccountID = @AccountID
	IF @Row > 0
		UPDATE ACCOUNT_CHAR SET bNation = @Nation WHERE strAccountID = @AccountID
	ELSE
		INSERT INTO ACCOUNT_CHAR (strAccountID, bNation ) VALUES (@AccountID, @Nation)

	SELECT @Row = COUNT(*) FROM  WAREHOUSE  WHERE strAccountID = @AccountID
	IF @Row = 0	
		INSERT INTO WAREHOUSE ( strAccountID ) VALUES (@AccountID)

	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  -2	
		RETURN
	END
	
COMMIT TRAN
SET @nRet =  1
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- Created by sungyong 2002.10.14

CREATE PROCEDURE RANK_KNIGHTS
AS

UPDATE KNIGHTS SET Points=0

SET NOCOUNT ON
DECLARE @KnightsIndex smallint
DECLARE @SumLoyalty int
DECLARE job1 CURSOR FOR

SELECT IDNum FROM KNIGHTS

OPEN job1
FETCH NEXT FROM job1
INTO @KnightsIndex
WHILE @@fetch_status = 0 
BEGIN

	SET @SumLoyalty = 0
	SELECT @SumLoyalty=Sum(Loyalty) FROM USERDATA WHERE Knights = @Knightsindex
	IF @SumLoyalty <> 0
		UPDATE KNIGHTS SET Points = @SumLoyalty WHERE IDNum = @knightsindex

	FETCH NEXT FROM job1
	INTO @KnightsIndex
END
CLOSE job1
DEALLOCATE job1
SET NOCOUNT OFF

-- ranking procedure call
EXEC KNIGHTS_RATING_UPDATE 

DECLARE @Knights_1 smallint
DECLARE @Knights_2 smallint
DECLARE @Knights_3 smallint
DECLARE @Knights_4 smallint
DECLARE @Knights_5 smallint

SELECT @Knights_1 = shIndex FROM KNIGHTS_RATING WHERE nRank=1
SELECT @Knights_2 = shIndex FROM KNIGHTS_RATING WHERE nRank=2
SELECT @Knights_3 = shIndex FROM KNIGHTS_RATING WHERE nRank=3
SELECT @Knights_4 = shIndex FROM KNIGHTS_RATING WHERE nRank=4
SELECT @Knights_5 = shIndex FROM KNIGHTS_RATING WHERE nRank=5

UPDATE KNIGHTS SET Ranking=1 WHERE IDNum=@Knights_1
UPDATE KNIGHTS SET Ranking=2 WHERE IDNum=@Knights_2
UPDATE KNIGHTS SET Ranking=3 WHERE IDNum=@Knights_3
UPDATE KNIGHTS SET Ranking=4 WHERE IDNum=@Knights_4
UPDATE KNIGHTS SET Ranking=5 WHERE IDNum=@Knights_5
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- sungyong 2002.08.22 create
-- modify 2002.09.28

CREATE PROCEDURE UPDATE_KNIGHTS

@nRet		smallint OUTPUT, 
@Type		tinyint,
@UserId 	char(21), 
@KnightsIndex	smallint,
@Domanation	tinyint

AS

DECLARE @Row tinyint 
DECLARE @Members tinyint
DECLARE @KnightsNumber smallint
DECLARE @ViceChief_1 char(21)
DECLARE @ViceChief_2 char(21)
DECLARE @ViceChief_3 char(21)
	SET @Row = 0
	SET @KnightsNumber = 0

	SELECT @Row = COUNT(*) FROM KNIGHTS WHERE IDNum = @KnightsIndex
	
	IF @Row = 0
	BEGIN
		SET @nRet =  7
		RETURN
	END

	SELECT @Members = Members, @ViceChief_1=ViceChief_1, @ViceChief_2=ViceChief_2, @ViceChief_3=ViceChief_3 FROM KNIGHTS WHERE IDNum = @KnightsIndex
	IF @Members >= 24
	BEGIN
		SET @nRet =  8
		RETURN
	END

	SELECT @Row = COUNT(*) FROM USERDATA WHERE strUserId = @UserId
	IF @Row = 0 
	BEGIN
		SET @nRet =  2
		RETURN
	END

--SELECT @ViceChief_1=ViceChief_1, @ViceChief_2=ViceChief_2, @ViceChief_3=ViceChief_3  FROM KNIGHTS WHERE IDNum = @KnightsIndex	

BEGIN TRAN	
	IF @Type = 18					-- JOIN
		BEGIN
			UPDATE KNIGHTS SET Members = Members + 1 WHERE IDNum = @KnightsIndex
			INSERT INTO KNIGHTS_USER ( sIDNum, strUserID  ) VALUES	 (@KnightsIndex,  @UserId )
		END
	ELSE IF @Type = 19 or @Type = 20 or @Type = 23	-- WITHDRAW, REMOVE, REJECT
		BEGIN
			IF @Members <= 1
				UPDATE KNIGHTS SET Members = 1 WHERE IDNum = @KnightsIndex
			ELSE 
				UPDATE KNIGHTS SET Members = Members - 1 WHERE IDNum = @KnightsIndex
			DELETE FROM KNIGHTS_USER WHERE strUserID = @UserId
			IF @ViceChief_1 = @UserId
				UPDATE KNIGHTS SET ViceChief_1 = NULL WHERE IDNum = @KnightsIndex	
			IF @ViceChief_2 = @UserId
				UPDATE KNIGHTS SET ViceChief_2 = NULL WHERE IDNum = @KnightsIndex	
			IF @ViceChief_3 = @UserId
				UPDATE KNIGHTS SET ViceChief_3 = NULL WHERE IDNum = @KnightsIndex	
		END
	ELSE IF @Type = 25				-- Change Chief, 부단장중에 한명이 단장으로 승격시에는 부단장 데이타변수에서 부단장 이름을 빼준다
		BEGIN
			UPDATE KNIGHTS SET Chief = @UserId WHERE IDNum = @KnightsIndex
			IF @ViceChief_1 = @UserId
				UPDATE KNIGHTS SET ViceChief_1 = NULL WHERE IDNum = @KnightsIndex	
			IF @ViceChief_2 = @UserId
				UPDATE KNIGHTS SET ViceChief_2 = NULL WHERE IDNum = @KnightsIndex	
			IF @ViceChief_3 = @UserId
				UPDATE KNIGHTS SET ViceChief_3 = NULL WHERE IDNum = @KnightsIndex	
		END
	ELSE IF @Type = 26				-- Change Vice Chief
		BEGIN
			IF @ViceChief_1 IS NOT NULL AND @ViceChief_2 IS NOT NULL AND @ViceChief_3 IS NOT NULL
			BEGIN
				SET @nRet =  8
				COMMIT TRAN
				RETURN
			END

			IF @ViceChief_1 IS NULL
				UPDATE KNIGHTS SET ViceChief_1 = @UserId WHERE IDNum = @KnightsIndex
			ELSE IF @ViceChief_2 IS NULL
				UPDATE KNIGHTS SET ViceChief_2 = @UserId WHERE IDNum = @KnightsIndex
			ELSE IF @ViceChief_3 IS NULL
				UPDATE KNIGHTS SET ViceChief_3 = @UserId WHERE IDNum = @KnightsIndex
		END
--	ELSE IF @Type = 27				-- Change Officer
	--	UPDATE KNIGHTS SET ViceChief_2 = @UserId WHERE IDNum = @KnightsIndex

	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  2
		RETURN
	END

	IF @Type = 20	-- REMOVE
		UPDATE USERDATA SET Knights = -1, Fame = 0  WHERE strUserId = @UserId
/*
	IF @Type = 18
		BEGIN
			SELECT @KnightsNumber = Knights FROM USERDATA WHERE strUserId = @UserId
			IF @KnightsNumber <> 0
			BEGIN
				ROLLBACK TRAN 
				SET @nRet =  5
				RETURN
			END
			UPDATE USERDATA SET Knights = @KnightsIndex, Fame = 5   WHERE strUserId = @UserId	-- TRAINEE
		END
	ELSE IF @Type = 19 or @Type = 20 or @Type = 23	-- WITHDRAW, REMOVE, REJECT
		UPDATE USERDATA SET Knights = 0, Fame = 0  WHERE strUserId = @UserId
	ELSE IF @Type = 22				-- ADMIT
		UPDATE USERDATA SET Fame = 3  WHERE strUserId = @UserId
	ELSE IF @Type = 24				-- Punish
		UPDATE USERDATA SET Fame = 6   WHERE strUserId = @UserId	
	ELSE IF @Type = 25				-- Change Chief
		UPDATE USERDATA SET Fame = 1  WHERE strUserId = @UserId
	ELSE IF @Type = 26				-- Change Vice Chief
		UPDATE USERDATA SET Fame = 2  WHERE strUserId = @UserId
	ELSE IF @Type = 27				-- Change Officer
		UPDATE USERDATA SET Fame = 4  WHERE strUserId = @UserId
		
	IF @@ERROR <> 0
	BEGIN	 
		ROLLBACK TRAN 
		SET @nRet =  5
		RETURN
	END	*/
	
COMMIT TRAN
SET @nRet =  0
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO


CREATE  PROCEDURE UPDATE_USER_DATA
	@id 		varchar(21),
	@Nation	tinyint,
	@Race		tinyint,
	@Class		smallint,
	@HairColor	tinyint,
	@Rank		tinyint,
	@Title		tinyint,
	@Level		tinyint,
	@Exp		int,
	@Loyalty	int,
	@Face		tinyint, 
	@City		tinyint,	
	@Knights	smallint,
	@Fame		tinyint,
	@Hp		smallint,
	@Mp		smallint, 
	@Sp		smallint,
	@Str		tinyint,
	@Sta		tinyint,
	@Dex		tinyint,
	@Intel		tinyint,
	@Cha		tinyint,
	@Authority	tinyint,
	@Points		tinyint,	
	@Gold		int,
	@Zone		tinyint,
	@Bind		smallint,
	@PX		int,
	@PZ		int,
	@PY		int,
	@dwTime	int,
	@strSkill	varchar(20),
	@strItem	varchar(400)
AS

DECLARE @KnightsIndex smallint

IF @Zone > 2		-- battle zone user
BEGIN
	SELECT @KnightsIndex=Knights FROM USERDATA WHERE strUserId=@id
	IF @KnightsIndex = -1	-- expel user
	BEGIN
		SET @Knights = 0
		SET @Fame = 0
	END
END

UPDATE	USERDATA
Set
	Nation		= @Nation,
	Race		= @Race,
	Class		= @Class,
	HairColor	= @HairColor,
	Rank		= @Rank,
	Title		= @Title,
	[Level]		= @Level,
	[Exp]		= @Exp,
	Loyalty		= @Loyalty,
	Face		= @Face, 
	City		= @City,	
	Knights		= @Knights,
	Fame		= @Fame,
	Hp		= @Hp,
	Mp		= @Mp, 
	Sp		= @Sp,
	Strong		= @Str,
	Sta		= @Sta,
	Dex		= @Dex,
	Intel		= @Intel,
	Cha		= @Cha,
	Authority	= @Authority,
	Points		= @Points,
	Gold		= @Gold,
	[Zone]		= @Zone,
	Bind		= @Bind,
	PX		= @PX,
	PZ		= @PZ,
	PY		= @PY,
	dwTime		= @dwTime,
	strSkill		= @strSkill,
	strItem		= @strItem
WHERE	strUserId	= @id
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO



CREATE  PROCEDURE UPDATE_WAREHOUSE
	@accountid 		varchar(21),
	@Money		int,
	@dwTime		int,
	@strItem		varchar(1600)
AS
UPDATE	WAREHOUSE
Set
	nMoney		= @Money,
	dwTime		= @dwTime,
	WarehouseData	= @strItem
WHERE	strAccountID	= @accountid
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

