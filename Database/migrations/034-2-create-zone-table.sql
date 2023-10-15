CREATE TABLE [dbo].[ZONE_INFO](
	[ServerNo] [tinyint] NOT NULL,
	[ZoneNo] [smallint] NOT NULL,
	[strZoneName] [char](30) NOT NULL,
	[InitX] [int] NOT NULL,
	[InitZ] [int] NOT NULL,
	[InitY] [int] NOT NULL,
	[Type] [tinyint] NOT NULL,
	[RoomEvent] [tinyint] NULL,
	[Comments] [char](60) NULL
) ON [PRIMARY];

INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 2, N'elmo2004.smd                  ', 163700, 41800, 38500, 1, 0, NULL)
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 1, N'karus2004.smd                 ', 37120, 157300, 38500, 1, 0, NULL)
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 101, N'battle_0810.smd               ', 23700, 14000, 38500, 3, 0, NULL)
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 102, N'battle_0810.smd               ', 23700, 14000, 38500, 3, 0, NULL)
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 71, N'freezone_b_20050718.smd       ', 1000, 1000, 0, 2, 0, N'Colony Zone Ronark Land                                     ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 21, N'moradon_050324tah.smd         ', 31200, 40200, 0, 1, 21, N'Moradon                                                     ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 11, N'k_eslant.smd                  ', 15000, 15000, 15000, 0, 0, N'Karus Eslant                                                ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 12, N'e_eslant.smd                  ', 15000, 15000, 15000, 0, 0, N'Elmorad Eslant                                              ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 30, N'war_a.smd                     ', 1000, 1000, 0, 2, 0, N'Delos Siege                                                 ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 32, N'abyssb_0925.smd               ', 1000, 1000, 0, 2, 0, N'Desperation Abyss                                           ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 33, N'dungeonc_1008.smd             ', 1000, 1000, 0, 2, 0, N'Hell Abyss                                                  ')
INSERT [dbo].[ZONE_INFO] ([ServerNo], [ZoneNo], [strZoneName], [InitX], [InitZ], [InitY], [Type], [RoomEvent], [Comments]) VALUES (1, 72, N'freezone_a.smd                ', 1000, 1000, 0, 2, 0, N'Ardream                                                     ')

ALTER TABLE [dbo].[ZONE_INFO] ADD  CONSTRAINT [DF_ZONE_INFO2_ServerNo]  DEFAULT ((1)) FOR [ServerNo]

ALTER TABLE [dbo].[ZONE_INFO] ADD  CONSTRAINT [DF_ZONE_INFO2_InitX]  DEFAULT ((1000)) FOR [InitX]

ALTER TABLE [dbo].[ZONE_INFO] ADD  CONSTRAINT [DF_ZONE_INFO2_InitZ]  DEFAULT ((1000)) FOR [InitZ]

ALTER TABLE [dbo].[ZONE_INFO] ADD  CONSTRAINT [DF_ZONE_INFO2_InitY]  DEFAULT ((0)) FOR [InitY]

ALTER TABLE [dbo].[ZONE_INFO] ADD  CONSTRAINT [DF_ZONE_INFO2_Type]  DEFAULT ((1)) FOR [Type]

