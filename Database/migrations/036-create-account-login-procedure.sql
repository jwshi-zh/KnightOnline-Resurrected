CREATE OR ALTER PROCEDURE [dbo].[ACCOUNT_LOGIN]
@AccountID	varchar(21),
@Password	varchar(13),
@nRet		smallint	OUTPUT

AS

DECLARE @pwd varchar(13)

SET @pwd = null

SELECT @pwd = strPasswd FROM [dbo].[TB_USER] WHERE strAccountID = @AccountID
IF @pwd IS null
BEGIN
	BEGIN TRAN
		INSERT INTO TB_USER (strAccountID, strPasswd, strSocNo)
		VALUES (@AccountID, @Password, 1)
	COMMIT TRAN
	SET @nRet = 1
	RETURN
END

ELSE IF @pwd <> @Password
BEGIN
	SET @nRet = 3 -- test
	RETURN
END

SET @nRet = 1 -- success

