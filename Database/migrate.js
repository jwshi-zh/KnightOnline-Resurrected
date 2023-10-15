const Sequelize = require('sequelize')
const { Umzug, SequelizeStorage } = require('umzug');
const fs = require('fs')

const sequelize = new Sequelize({
  dialect: 'mssql',
  dialectModulePath: 'msnodesqlv8/lib/sequelize',
  dialectOptions: {
    user: 'kn_online',
    password: 'kn_online',
    options: {
      connectionString: 'Driver={ODBC Driver 17 for SQL Server};Server=(local);Database=kn_online;Trusted_Connection=yes;',
      trustedConnection: true,
    }
  },
  pool: {
    min: 0,
    max: 5,
    idle: 10000
  }
})

const umzug = new Umzug({
  migrations: {
    glob: 'migrations/*.{js,ts,sql}',
    resolve: ({ name, path, context: sequelize }) => ({
      name,
      up: async () => {
        const sql = fs.readFileSync(path).toString()
        return sequelize.query(sql)
      },
      down: async () => {
        // Get the corresponding `.down.sql` file to undo this migration
        const sql = fs.readFileSync(path.replace('.up.sql', '.down.sql')).toString()
        return sequelize.query(sql)
      }
    })
  },
  context: sequelize,
  storage: new SequelizeStorage({ sequelize }),
  logger: console,
});

(async () => {
  // Checks migrations and run them if they are not already applied. To keep
  // track of the executed migrations, a table (and sequelize model) called SequelizeMeta
  // will be automatically created (if it doesn't exist already) and parsed.
  await umzug.up();
})();
