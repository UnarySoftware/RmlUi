using Flax.Build;

public class RmlUiTarget : GameProjectTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        Platforms = new[]
        {
            TargetPlatform.Windows,
            TargetPlatform.Linux,
            TargetPlatform.Mac,
        };

        Modules.Add("RmlUi");
    }
}
