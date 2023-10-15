CREATE TABLE [dbo].[VERSION](
	[sVersion] [smallint] NOT NULL,
	[strFileName] [varchar](50) NOT NULL,
	[strCompressName] [varchar](50) NOT NULL,
	[sHistoryVersion] [smallint] NOT NULL
) ON [PRIMARY]

INSERT [dbo].[VERSION] ([sVersion], [strFileName], [strCompressName], [sHistoryVersion]) VALUES (1299, N'patch1299.zip', N'Sohu.Com', 0)
