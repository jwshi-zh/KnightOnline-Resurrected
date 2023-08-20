if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CURRENTUSER]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[CURRENTUSER]
GO

CREATE TABLE [dbo].[CURRENTUSER] (
	[strAccountID] [varchar] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strCharID] [varchar] (21) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[bServerGroup] [tinyint] NOT NULL ,
	[nServerNo] [smallint] NOT NULL ,
	[strServerIP] [varchar] (20) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strClientIP] [varchar] (20) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[timeLogin] [smalldatetime] NOT NULL ,
	[bBillingType] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[CURRENTUSER] WITH NOCHECK ADD 
	CONSTRAINT [PK_CURRENTUSER] PRIMARY KEY  CLUSTERED 
	(
		[strAccountID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[CURRENTUSER] WITH NOCHECK ADD 
	CONSTRAINT [DF_CURRENT_USER_bServerGroup] DEFAULT (0) FOR [bServerGroup],
	CONSTRAINT [DF_CURRENTUSER_timeLogin] DEFAULT (getdate()) FOR [timeLogin],
	CONSTRAINT [DF_CURRENTUSER_bBillingType] DEFAULT (0) FOR [bBillingType]
GO

