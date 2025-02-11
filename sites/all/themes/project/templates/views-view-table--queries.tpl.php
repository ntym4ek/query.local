<?php

/**
 * @file
 * Template to display a view as a table.
 *
 * Available variables:
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $caption: The caption for this table. May be empty.
 * - $header_classes: An array of header classes keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $classes: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * - $field_classes: An array of classes to apply to each field, indexed by
 *   field id, then row number. This matches the index in $rows.
 *
 * @ingroup templates
 */

$classes = str_replace('table-striped', '', $classes);
?>

<div class="dtable<?php print $classes ? ' ' . $classes : ''; ?><?php print $attributes; ?>">


  <?php if (!empty($header)) : ?>
    <div class="dhead">
      <?php foreach ($header as $field => $label): ?>
        <div class="dcol<?php print $header_classes[$field] ? ' ' . $header_classes[$field] : ''; ?>">
          <?php print $label; ?>
        </div>
      <?php endforeach; ?>
    </div>
  <?php endif; ?>

  <? $nom = ''; ?>
  <?php foreach ($rows as $index => $row): ?>
      <?
        if ($nom != $row["field_nomenklatura"]) {
          $nom = $row["field_nomenklatura"];
          $first = true; $last = false;
          $id = $result[$index]->id;
        } else {
          $first = false;
        }
        if (empty($rows[$index+1]) || $nom != $rows[$index+1]["field_nomenklatura"]) {
          $last = true;
        }
      ?>
      <div class="drow<?php print !$first ? ' arch' : ''; ?>">
        <?php foreach ($row as $field => $content): ?>
          <div class="dcol<?php print $field_classes[$field][$index] ? ' ' . $field_classes[$field][$index] : ''; ?><?php print drupal_attributes($field_attributes[$field][$index]); ?>">
            <? print !$first && $field == 'field_nomenklatura' ? '' : $content; ?>
          </div>
        <?php endforeach; ?>
      </div>

      <? if ($first): ?>
        <div class="collapse" id="collapse-<? print $id; ?>"><div>
      <? endif; ?>

      <? if ($last): ?>
        </div></div>
      <? endif; ?>

    <?php endforeach; ?>
</div>
