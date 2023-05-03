using System.IO;
using Flax.Build;
using Flax.Build.NativeCpp;

/// <summary>
/// RmlUi SDK
/// </summary>
public class RmlUi : GameModule
{
    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);
	
		BuildNativeCode = true;
		
		options.PrivateDependencies.Add("freetype");
		options.PublicIncludePaths.Add(FolderPath);
		
        var binariesFolder = Path.Combine(FolderPath, "Binaries", options.Platform.Target.ToString());
        switch (options.Platform.Target)
        {
        case TargetPlatform.Windows:
			options.LinkEnv.InputFiles.Add(Path.Combine(binariesFolder, "RmlCore.lib"));
			options.LinkEnv.InputFiles.Add(Path.Combine(binariesFolder, "RmlDebugger.lib"));
            break;
        case TargetPlatform.Linux:
        case TargetPlatform.Mac:
			options.LinkEnv.InputFiles.Add(Path.Combine(binariesFolder, "libRmlCore.a"));
            options.DependencyFiles.Add(Path.Combine(binariesFolder, "libRmlCore.a"));
            options.LinkEnv.InputFiles.Add(Path.Combine(binariesFolder, "libRmlDebugger.a"));
            options.DependencyFiles.Add(Path.Combine(binariesFolder, "libRmlDebugger.a"));
            break;
        }
		
    }
}
