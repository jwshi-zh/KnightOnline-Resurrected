CREATE TABLE [dbo].[MAKE_ITEM_GRADECODE](
	[byItemIndex] [tinyint] NOT NULL,
	[byGrade_1] [tinyint] NOT NULL,
	[byGrade_2] [tinyint] NULL,
	[byGrade_3] [tinyint] NULL,
	[byGrade_4] [tinyint] NULL,
	[byGrade_5] [tinyint] NULL,
	[byGrade_6] [tinyint] NULL,
	[byGrade_7] [tinyint] NULL,
	[byGrade_8] [tinyint] NULL,
	[byGrade_9] [tinyint] NULL
) ON [PRIMARY]

INSERT [dbo].[MAKE_ITEM_GRADECODE] ([byItemIndex], [byGrade_1], [byGrade_2], [byGrade_3], [byGrade_4], [byGrade_5], [byGrade_6], [byGrade_7], [byGrade_8], [byGrade_9]) VALUES (1, 1, 1, 1, 1, 1, 1, 1, 1, 1)
INSERT [dbo].[MAKE_ITEM_GRADECODE] ([byItemIndex], [byGrade_1], [byGrade_2], [byGrade_3], [byGrade_4], [byGrade_5], [byGrade_6], [byGrade_7], [byGrade_8], [byGrade_9]) VALUES (2, 1, 1, 1, 1, 1, 1, 1, 1, 1)

