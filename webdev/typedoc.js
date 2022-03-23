module.exports = {
    mode: 'modules',
    out: '../docs/webdev-html/',
    exclude: ['**/node_modules/**', '**/dist/**', '**/scripts/**', '**/docs/**'],
    name: 'cocr-react',
    excludePrivate: true,
    excludeExternals: true,
};