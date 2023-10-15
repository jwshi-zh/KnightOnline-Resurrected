CREATE OR ALTER PROCEDURE [dbo].[LOAD_KNIGHTS_MEMBERS]
@knightsindex	smallint
AS

SELECT strUserId, Fame, [Level], Class FROM USERDATA WHERE Knights = @knightsindex

--SET @nRet = 1
--RETURN


