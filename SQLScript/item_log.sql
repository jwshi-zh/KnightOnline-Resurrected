IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Knight_Log')
	DROP DATABASE [Knight_Log]
GO

CREATE DATABASE [Knight_Log]  ON (NAME = N'Knight_Log_Data', FILENAME = N'E:\Knight_Log_Data.MDF' , SIZE = 1, FILEGROWTH = 10%) LOG ON (NAME = N'Knight_Log_Log', FILENAME = N'E:\Knight_Log_Log.LDF' , SIZE = 1, FILEGROWTH = 0%)
 COLLATE Korean_Wansung_CI_AS
GO

exec sp_dboption N'Knight_Log', N'autoclose', N'false'
GO

exec sp_dboption N'Knight_Log', N'bulkcopy', N'false'
GO

exec sp_dboption N'Knight_Log', N'trunc. log', N'false'
GO

exec sp_dboption N'Knight_Log', N'torn page detection', N'true'
GO

exec sp_dboption N'Knight_Log', N'read only', N'false'
GO

exec sp_dboption N'Knight_Log', N'dbo use', N'false'
GO

exec sp_dboption N'Knight_Log', N'single', N'false'
GO

exec sp_dboption N'Knight_Log', N'autoshrink', N'false'
GO

exec sp_dboption N'Knight_Log', N'ANSI null default', N'false'
GO

exec sp_dboption N'Knight_Log', N'recursive triggers', N'false'
GO

exec sp_dboption N'Knight_Log', N'ANSI nulls', N'false'
GO

exec sp_dboption N'Knight_Log', N'concat null yields null', N'false'
GO

exec sp_dboption N'Knight_Log', N'cursor close on commit', N'false'
GO

exec sp_dboption N'Knight_Log', N'default to local cursor', N'false'
GO

exec sp_dboption N'Knight_Log', N'quoted identifier', N'false'
GO

exec sp_dboption N'Knight_Log', N'ANSI warnings', N'false'
GO

exec sp_dboption N'Knight_Log', N'auto create statistics', N'true'
GO

exec sp_dboption N'Knight_Log', N'auto update statistics', N'true'
GO

use [Knight_Log]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[DELETE_LOG]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[DELETE_LOG]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ITEM_LOG]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ITEM_LOG]
GO

if not exists (select * from master.dbo.syslogins where loginname = N'DB\Administrator')
	exec sp_grantlogin N'DB\Administrator'
	exec sp_defaultdb N'DB\Administrator', N'master'
	exec sp_defaultlanguage N'DB\Administrator', N'한국어'
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
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'Repent', @loginlang = N'한국어'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'repent', null, @logindb, @loginlang
END
GO

exec sp_addsrvrolemember N'DB\Administrator', sysadmin
GO

CREATE TABLE [dbo].[ITEM_LOG] (
	[strSrcID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strTarID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[bType] [tinyint] NOT NULL ,
	[nPutMoney] [int] NOT NULL ,
	[strPutItem] [varchar] (200) COLLATE Korean_Wansung_CI_AS NULL ,
	[nGetMoney] [int] NOT NULL ,
	[strGetItem] [varchar] (200) COLLATE Korean_Wansung_CI_AS NULL ,
	[TradeDate] [smalldatetime] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[ITEM_LOG] WITH NOCHECK ADD 
	CONSTRAINT [DF_ITEM_LOG_TradeDate] DEFAULT (getdate()) FOR [TradeDate]
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE [DELETE_LOG] AS

DECLARE @currenttime smalldatetime

SET @currenttime = getdate()

DELETE FROM ITEM_LOG WHERE ( DATEDIFF( dd, TradeDate, @currenttime ) > 15 )
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

