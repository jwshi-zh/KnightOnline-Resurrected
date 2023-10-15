CREATE TABLE [dbo].[TB_USER](
	[strAccountID] [varchar](21) NOT NULL,
	[strPasswd] [varchar](13) NOT NULL,
	[strSocNo] [char](15) NULL,
	[iDays] [int] NULL
) ON [PRIMARY]

ALTER TABLE [dbo].[TB_USER] ADD  CONSTRAINT [DF_TB_USER_strSocNo]  DEFAULT ((1)) FOR [strSocNo]

ALTER TABLE [dbo].[TB_USER] ADD  CONSTRAINT [DF_TB_USER_iDays]  DEFAULT ((6)) FOR [iDays]

