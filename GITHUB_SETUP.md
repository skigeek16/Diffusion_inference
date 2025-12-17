# Quick GitHub Setup

## Create Repository on GitHub

1. Go to https://github.com/new
2. Repository name: `stable-diffusion-gui-cpp`
3. Description: `Modern C++ GUI for Stable Diffusion inference with ImGui - Optimized for H100 GPU`
4. Keep it Public (or Private if you prefer)
5. **DO NOT** initialize with README, .gitignore, or license
6. Click "Create repository"

## Push to GitHub

After creating the repository, run these commands:

```powershell
# Replace YOUR_USERNAME with your GitHub username
git remote add origin https://github.com/YOUR_USERNAME/stable-diffusion-gui-cpp.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## Or use GitHub CLI (if installed)

```powershell
# Login
gh auth login

# Create and push
gh repo create stable-diffusion-gui-cpp --public --source=. --remote=origin --push
```

## Repository is Ready!

Your local repository is committed and ready to push.
The repository includes:

✅ Source code (src/)
✅ CMakeLists.txt
✅ README.md
✅ Lightning AI setup guide
✅ .gitignore (excludes builds, models)
✅ Build scripts

**Note**: Models are NOT included (too large). Users will download them separately.
