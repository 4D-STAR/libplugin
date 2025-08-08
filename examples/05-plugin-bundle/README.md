# 05-Plugin-Bundle Example

Generally when using libplugin the intended method of use is to 

- Create a plugin or set of plugins
- Bundle them together using the `fourdst-cli bundle` tool
- Fill that bundle with many precompiled dynamic libraries for a variety of platforms using the `fourdst-cli bundle fill` tool
- Sign that bundle using the `fourdst-cli bundle sign` tool
- Distribute that bundle to users


Using a bundle brings many benefits:
- Simplifies distribution of multiple plugins
- Ensures compatibility across platforms
- Provides a mechanism for verifying authenticity and integrity of plugins
- Allows for easy updates and versioning of plugins
- Makes it easier for plugin hosts to discover and load available plugins.

This example demonstrates how to load a already extant plugin bundle

An example bundle is provided in assets (example.fbundle) which has had dynamic libraries for linux (x86_64 and aarch64)
and macos (arm64) platforms precompiled and included in the bundle.

Further the public key associated to the signature of the bundle is included in the assets directory.

In order to run the example you will need to install the public key to your fourdst keyring. This lives at
`$HOME/.config/fourdst/keys`. You can either manually move the `assets/example.pub.pem` file to that directory or run
the following command (if you have the fourdst-cli tool installed):

```bash
fourdst-cli key add assets/example.pub.pem
```

Then running the example is as simple as

```bash
./build/plugin_bundle assets/example.fbundle
```