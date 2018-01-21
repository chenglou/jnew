title: Pack.re
description: A reason-native javascript bundler 🏎
status: alpha
tags: tool, javascript, reason
---
date: 2018-9-11

I did some benchmarking, compiling [gravitron](/projects/gravitron/), and pack.re was pretty fast 😄

benchmarked compiling gravitron: parcel 3.0s, webpack 1.25s, pack.re 0.2s 🏎 🚀 node takes 0.3 seconds just in startup Is it 6-10x faster because of native compilation, or because it has no features? 🤔 NB:I'm not saying we should ditch JS build tools, webpack& parcel are great😄

- webpack (no pre/post-processing, simplest config) - 333.eee
- parcel (no cache, no minify) - 3333.2222
- pack.re - 0.2s 🚀

That's 6-10x faster! (Note: webpack & parcel have tons more features, and pack.re probably still has some bugs)

Also of note:
- `time node -e 'console.log("hi")'` - 0.3s 🙃

---
date: 2018-1-11

I was working on [reprocessing-scripts](/projects/reprocessing-scripts), and I didn't want to have to depend on some huuge javascript bundler w/ all of their dependencies (webpack's `node_modules` is 50megs, parcel's `node_modules` is 150megs), so I figured I'd try making a reason-native bundler.

I repurposed flow's [javascript parser](), inspired by [jeason](), and figured out the node resolution algorithm, and it wasn't too much work after that.

I also managed to get hot-reloading working! I added a `--external-everything` flag which creates a bundle that only contains the files in your project, and no external dependencies. It expects them to already have been loaded on the page by a previous `pack.re` bundle.