CREATE OR ALTER PROCEDURE [dbo].[UPDATE_WAREHOUSE]
	@accountid 		varchar(21),
	@Money		int,
	@dwTime		int,
	@strItem		char(1600),
	@strSerial               char(20)
AS
UPDATE	WAREHOUSE
Set
	nMoney		= @Money,
	dwTime		= @dwTime,
	WarehouseData	= @strItem,
                strSerial		=@strSerial
WHERE	strAccountID	= @accountid


