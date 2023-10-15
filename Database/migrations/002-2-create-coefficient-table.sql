CREATE TABLE [dbo].[COEFFICIENT](
	[sClass] [smallint] NOT NULL,
	[ShortSword] [float] NOT NULL,
	[Sword] [float] NOT NULL,
	[Axe] [float] NOT NULL,
	[Club] [float] NOT NULL,
	[Spear] [float] NOT NULL,
	[Pole] [float] NOT NULL,
	[Staff] [float] NOT NULL,
	[Bow] [float] NULL,
	[Hp] [float] NOT NULL,
	[Mp] [float] NOT NULL,
	[Sp] [float] NOT NULL,
	[Ac] [float] NOT NULL,
	[Hitrate] [float] NOT NULL,
	[Evasionrate] [float] NOT NULL
) ON [PRIMARY];

INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (101, 0.0001, 0.00013, 0.00013, 0.00013, 0.00013, 0.00013, 0.0001, 0.0001, 0.0015, 0, 0.0015, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (102, 0.00015, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0005, 0, 0.0015, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (103, 7E-05, 0, 0, 0, 0, 0, 0.0001, 0, 0.0004, 0.0015, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (104, 0, 0, 0, 5E-05, 0, 0, 0.0001, 0, 0.001, 0.0015, 0, 1.05, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (105, 0.0002, 0.00025, 0.00025, 0.00023, 0.00025, 0.00025, 0.0001, 0.0001, 0.003, 0, 0.003, 1.2, 0.015, 0.0125)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (106, 0.0002, 0.00032, 0.00032, 0.00013, 0.00032, 0.00032, 0.0001, 0.0001, 0.004, 0, 0.003, 1.26, 0.02, 0.0132)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (107, 0.00025, 0.0001, 0.0001, 0.0001, 0.0002, 0.0001, 0.0001, 0.00031, 0.0015, 0, 0.003, 1.01, 0.03, 0.025)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (108, 0.00032, 0.0001, 0.0001, 0.0001, 0.0002, 0.0001, 0.0001, 0.000325, 0.00165, 0, 0.003, 1.012, 0.06, 0.025)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (109, 0.00015, 0, 0, 0, 0, 0, 0.00025, 0, 0.0008, 0.0015, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (110, 0.00015, 0, 0, 0, 0, 0, 0.0003, 0, 0.0008, 0.0018, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (111, 0, 0, 0, 0.0003, 0, 0.0001, 0.0001, 0, 0.0012, 0.0015, 0, 1.05, 0.014, 0.014)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (112, 0.00015, 0.00025, 0.0001, 0.000315, 0.0001, 0.0001, 0.00015, 0, 0.00157, 0.0018, 0, 1.2, 0.014, 0.0145)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (201, 0.0001, 0.00013, 0.00013, 0.00013, 0.00013, 0.00013, 0.0001, 0.0001, 0.0015, 0, 0.0015, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (202, 0.00015, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0005, 0, 0.0015, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (203, 7E-05, 0, 0, 0, 0, 0, 0.0001, 0, 0.0004, 0.0015, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (204, 0, 0, 0, 5E-05, 0, 0, 0.0001, 0, 0.001, 0.0015, 0, 1.05, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (205, 0.0002, 0.00025, 0.00025, 0.00023, 0.00025, 0.00025, 0.0001, 0.0001, 0.003, 0, 0.003, 1.2, 0.015, 0.0125)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (206, 0.0002, 0.00032, 0.00032, 0.00013, 0.00032, 0.00032, 0.0001, 0.0001, 0.004, 0, 0.003, 1.26, 0.02, 0.0132)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (207, 0.00025, 0.0001, 0.0001, 0.0001, 0.0002, 0.0001, 0.0001, 0.00031, 0.0015, 0, 0.003, 1.01, 0.03, 0.025)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (208, 0.00032, 0.0001, 0.0001, 0.0001, 0.0002, 0.0001, 0.0001, 0.000325, 0.00165, 0, 0.003, 1.012, 0.06, 0.025)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (209, 0.00015, 0, 0, 0, 0, 0, 0.00025, 0, 0.0008, 0.0015, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (210, 0.00015, 0, 0, 0, 0, 0, 0.0003, 0, 0.0008, 0.0018, 0, 1, 0.01, 0.01)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (211, 0, 0, 0, 0.0003, 0, 0.0001, 0.0001, 0, 0.0012, 0.0015, 0, 1.05, 0.014, 0.014)
INSERT [dbo].[COEFFICIENT] ([sClass], [ShortSword], [Sword], [Axe], [Club], [Spear], [Pole], [Staff], [Bow], [Hp], [Mp], [Sp], [Ac], [Hitrate], [Evasionrate]) VALUES (212, 0.00015, 0.00025, 0.0001, 0.000315, 0.0001, 0.0001, 0.00015, 0, 0.00157, 0.0018, 0, 1.2, 0.014, 0.0145)
