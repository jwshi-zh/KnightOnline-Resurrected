CREATE TABLE [dbo].[USERDATA](
	[strUserId] [char](21) NOT NULL,
	[Nation] [tinyint] NOT NULL,
	[Race] [tinyint] NOT NULL,
	[Class] [smallint] NOT NULL,
	[HairColor] [tinyint] NOT NULL,
	[Rank] [tinyint] NOT NULL,
	[Title] [tinyint] NOT NULL,
	[Level] [tinyint] NOT NULL,
	[Exp] [int] NOT NULL,
	[Loyalty] [int] NOT NULL,
	[Face] [tinyint] NOT NULL,
	[City] [tinyint] NOT NULL,
	[Knights] [smallint] NOT NULL,
	[Fame] [tinyint] NOT NULL,
	[Hp] [smallint] NOT NULL,
	[Mp] [smallint] NOT NULL,
	[Sp] [smallint] NOT NULL,
	[Strong] [tinyint] NOT NULL,
	[Sta] [tinyint] NOT NULL,
	[Dex] [tinyint] NOT NULL,
	[Intel] [tinyint] NOT NULL,
	[Cha] [tinyint] NOT NULL,
	[Authority] [tinyint] NOT NULL,
	[Points] [tinyint] NOT NULL,
	[Gold] [int] NOT NULL,
	[Zone] [tinyint] NOT NULL,
	[Bind] [smallint] NULL,
	[PX] [int] NOT NULL,
	[PZ] [int] NOT NULL,
	[PY] [int] NOT NULL,
	[dwTime] [int] NOT NULL,
	[strSkill] [varchar](20) NULL,
	[strItem] [varchar](400) NULL,
	[strSerial] [varchar](400) NULL,
 CONSTRAINT [PK_USERDATA] PRIMARY KEY CLUSTERED 
(
	[strUserId] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Nation]  DEFAULT ((0)) FOR [Nation]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Race]  DEFAULT ((1)) FOR [Race]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Class]  DEFAULT ((0)) FOR [Class]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Gender]  DEFAULT ((0)) FOR [HairColor]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Rank]  DEFAULT ((0)) FOR [Rank]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Title]  DEFAULT ((0)) FOR [Title]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Level]  DEFAULT ((1)) FOR [Level]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Exp]  DEFAULT ((0)) FOR [Exp]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Loyalty]  DEFAULT ((0)) FOR [Loyalty]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Religion]  DEFAULT ((0)) FOR [Face]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_City]  DEFAULT ((0)) FOR [City]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Knights]  DEFAULT ((0)) FOR [Knights]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Fame]  DEFAULT ((0)) FOR [Fame]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Hp]  DEFAULT ((100)) FOR [Hp]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Mp]  DEFAULT ((100)) FOR [Mp]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Sp]  DEFAULT ((100)) FOR [Sp]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Str]  DEFAULT ((0)) FOR [Strong]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Sta]  DEFAULT ((0)) FOR [Sta]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Dex]  DEFAULT ((0)) FOR [Dex]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Intel]  DEFAULT ((0)) FOR [Intel]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Cha]  DEFAULT ((0)) FOR [Cha]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Authority]  DEFAULT ((1)) FOR [Authority]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Points]  DEFAULT ((0)) FOR [Points]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Gold]  DEFAULT ((0)) FOR [Gold]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_Zone]  DEFAULT ((1)) FOR [Zone]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_PX]  DEFAULT ((268100)) FOR [PX]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_PZ]  DEFAULT ((131000)) FOR [PZ]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_PY]  DEFAULT ((0)) FOR [PY]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_dwTime]  DEFAULT ((0)) FOR [dwTime]

ALTER TABLE [dbo].[USERDATA] ADD  CONSTRAINT [DF_USERDATA_strSkill]  DEFAULT (0x00) FOR [strSkill]
