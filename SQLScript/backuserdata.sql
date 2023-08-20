if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BK_MON_USERDATA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BK_MON_USERDATA]
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

ALTER TABLE [dbo].[BK_MON_USERDATA] WITH NOCHECK ADD 
	CONSTRAINT [PK_BK_MON_USERDATA] PRIMARY KEY  CLUSTERED 
	(
		[strUserID]
	)  ON [PRIMARY] 
GO

