CREATE TABLE [dbo].[MAKE_ITEM](
	[sIndex] [smallint] NOT NULL,
	[strItemInfo] [char](20) NULL,
	[iItemCode] [int] NOT NULL,
	[byItemLevel] [tinyint] NOT NULL
) ON [PRIMARY]

ALTER TABLE [dbo].[MAKE_ITEM] ADD  CONSTRAINT [DF_MAKE_ITEM_byItemLevel]  DEFAULT ((0)) FOR [byItemLevel]

