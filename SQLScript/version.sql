if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[VERSION]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[VERSION]
GO

CREATE TABLE [dbo].[VERSION] (
	[sVersion] [smallint] NOT NULL ,
	[strFileName] [char] (255) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[strCompressName] [char] (255) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[sHistoryVersion] [smallint] NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[VERSION] WITH NOCHECK ADD 
	CONSTRAINT [PK_VERSION] PRIMARY KEY  CLUSTERED 
	(
		[strFileName]
	)  ON [PRIMARY] 
GO

