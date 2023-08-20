if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CONNECTTIME_LOG]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[CONNECTTIME_LOG]
GO

if not exists (select * from master.dbo.syslogins where loginname = N'knight')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'Knight_Account', @loginlang = N'한국어'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'knight', null, @logindb, @loginlang
END
GO

if not exists (select * from dbo.sysusers where name = N'knight' and uid < 16382)
	EXEC sp_grantdbaccess N'knight', N'knight'
GO

exec sp_addrolemember N'db_owner', N'knight'
GO

CREATE TABLE [dbo].[CONNECTTIME_LOG] (
	[strAccountID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strCharID] [char] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[timeLogin] [smalldatetime] NOT NULL ,
	[timeLogout] [smalldatetime] NOT NULL ,
	[nDuring] [smallint] NOT NULL ,
	[strServerIP] [char] (20) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strClientIP] [char] (20) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[bBillingType] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[CONNECTTIME_LOG] WITH NOCHECK ADD 
	CONSTRAINT [DF_CONNECTTIME_LOG_bBillingType] DEFAULT (0) FOR [bBillingType]
GO

