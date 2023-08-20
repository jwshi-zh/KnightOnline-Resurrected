if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CONNECTTIME_LOG]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[CONNECTTIME_LOG]
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

