CREATE OR ALTER PROCEDURE [dbo].[LOAD_ACCOUNT_CHARID]
@Accountid 	char(21)
AS

SELECT strCharID1, strCharID2, strCharID3, strCharID4, strCharID5  FROM ACCOUNT_CHAR WHERE strAccountID = @Accountid

RETURN @@ROWCOUNT


