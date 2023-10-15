CREATE TABLE [dbo].[MAKE_ITEM_LARECODE](
	[byLevelGrade] [tinyint] NOT NULL,
	[sLareItem] [smallint] NOT NULL,
	[sMagicItem] [smallint] NOT NULL,
	[sGereralItem] [smallint] NOT NULL
) ON [PRIMARY]

INSERT [dbo].[MAKE_ITEM_LARECODE] ([byLevelGrade], [sLareItem], [sMagicItem], [sGereralItem]) VALUES (1, 20, 1, 1)
INSERT [dbo].[MAKE_ITEM_LARECODE] ([byLevelGrade], [sLareItem], [sMagicItem], [sGereralItem]) VALUES (2, 21, 1, 1)

ALTER TABLE [dbo].[MAKE_ITEM_LARECODE] ADD  CONSTRAINT [DF_MAKE_ITEM_LARECODE_sLareItem]  DEFAULT ((0)) FOR [sLareItem]

ALTER TABLE [dbo].[MAKE_ITEM_LARECODE] ADD  CONSTRAINT [DF_MAKE_ITEM_LARECODE_sMagicItem]  DEFAULT ((0)) FOR [sMagicItem]

ALTER TABLE [dbo].[MAKE_ITEM_LARECODE] ADD  CONSTRAINT [DF_MAKE_ITEM_LARECODE_sGereralItem]  DEFAULT ((0)) FOR [sGereralItem]

